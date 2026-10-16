/*
 * Copyright (C) 2021-2026 CutefishOS Team.
 * Copyright (C) 2026 Project Aevis Contributors.
 *
 * Author:     cutefish <cutefishos@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "windowblur.h"
#include "waylandblurmanager.h"

#include <QEvent>
#include <QMetaObject>
#include <QPlatformSurfaceEvent>
#include <QWindow>

#include <KWayland/Client/surface.h>

using namespace KWayland::Client;

namespace {

// A rounded rectangle as a region: the two overlapping bars that make up the
// straight sides, plus a circle in each corner. Without the corners the blur
// squares off the window's own rounded edges, which is exactly where it shows.
QRegion roundedRegion(const QRect &rect, int radius)
{
    if (radius <= 0)
        return QRegion(rect);

    radius = qMin(radius, qMin(rect.width(), rect.height()) / 2);

    QRegion region(rect.adjusted(radius, 0, -radius, 0));
    region += QRegion(rect.adjusted(0, radius, 0, -radius));

    const int diameter = radius * 2;
    region += QRegion(rect.left(), rect.top(), diameter, diameter, QRegion::Ellipse);
    region += QRegion(rect.right() - diameter + 1, rect.top(), diameter, diameter, QRegion::Ellipse);
    region += QRegion(rect.left(), rect.bottom() - diameter + 1, diameter, diameter, QRegion::Ellipse);
    region += QRegion(rect.right() - diameter + 1, rect.bottom() - diameter + 1,
                      diameter, diameter, QRegion::Ellipse);

    return region;
}

} // namespace

WindowBlur::WindowBlur(QObject *parent) noexcept
    : QObject(parent)
{
    connect(WaylandBlurManager::instance(), &WaylandBlurManager::ready,
            this, &WindowBlur::scheduleUpdate);
}

WindowBlur::~WindowBlur()
{
    forget();
}

void WindowBlur::classBegin()
{
}

void WindowBlur::componentComplete()
{
    m_complete = true;
    updateBlur();
}

void WindowBlur::setView(QWindow *view)
{
    if (m_view == view)
        return;

    if (m_view)
        m_view->removeEventFilter(this);

    forget();
    m_view = view;

    if (m_view) {
        // Qt destroys the wl_surface whenever the window is hidden and makes a
        // new one on the next expose, and blur is surface state: it has to be
        // set again on the new surface, not once for the window.
        m_view->installEventFilter(this);
        connect(m_view, &QWindow::widthChanged, this, &WindowBlur::scheduleUpdate, Qt::UniqueConnection);
        connect(m_view, &QWindow::heightChanged, this, &WindowBlur::scheduleUpdate, Qt::UniqueConnection);
        connect(m_view, &QWindow::visibleChanged, this, &WindowBlur::scheduleUpdate, Qt::UniqueConnection);
    }

    Q_EMIT viewChanged();
    scheduleUpdate();
}

QWindow *WindowBlur::view() const
{
    return m_view;
}

void WindowBlur::setEnabled(bool enabled)
{
    if (enabled == m_enabled)
        return;

    m_enabled = enabled;
    scheduleUpdate();
    Q_EMIT enabledChanged();
}

bool WindowBlur::enabled() const
{
    return m_enabled;
}

void WindowBlur::setWindowRadius(qreal radius)
{
    if (qFuzzyCompare(radius, m_windowRadius))
        return;

    m_windowRadius = radius;
    scheduleUpdate();
    Q_EMIT windowRadiusChanged();
}

qreal WindowBlur::windowRadius() const
{
    return m_windowRadius;
}

bool WindowBlur::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_view) {
        if (event->type() == QEvent::Expose) {
            updateBlur();
        } else if (event->type() == QEvent::PlatformSurface) {
            const auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
            if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
                forget();
            } else {
                // The QWindow surface is not guaranteed to be available until
                // the platform-surface event has finished dispatching.
                QMetaObject::invokeMethod(this, [this] { updateBlur(); }, Qt::QueuedConnection);
            }
        } else if (event->type() == QEvent::Hide) {
            forget();
        }
    }

    return QObject::eventFilter(watched, event);
}

void WindowBlur::scheduleUpdate()
{
    updateBlur();
}

QRegion WindowBlur::blurRegion() const
{
    if (!m_enabled || !m_view || !m_view->isVisible())
        return QRegion();

    // Surface local coordinates, in logical pixels: what the compositor blurs
    // is the window, wherever it happens to be.
    return roundedRegion(QRect(QPoint(0, 0), m_view->size()), qRound(m_windowRadius));
}

void WindowBlur::forget()
{
    if (m_surface) {
        WaylandBlurManager::instance()->removeBlur(m_surface);
        m_surface = nullptr;
    }
    m_region = QRegion();
}

void WindowBlur::updateBlur()
{
    if (!m_complete || !m_view)
        return;

    WaylandBlurManager *manager = WaylandBlurManager::instance();
    if (!manager->isValid())
        return;

    if (!m_enabled || !m_view->isVisible()) {
        forget();
        return;
    }

    Surface *surface = Surface::fromWindow(m_view);
    if (!surface)
        return;

    if (surface != m_surface) {
        forget();
        m_surface = surface;
    }

    const QRegion region = blurRegion();
    if (region == m_region)
        return;

    m_region = region;
    manager->updateBlur(surface, region);
    m_view->requestUpdate();
}
