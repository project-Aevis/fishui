#include "waylandblurmanager.h"
#include "ext-background-effect-v1-client-protocol.h"

#include <QGuiApplication>
#include <QDebug>

#include <KWayland/Client/blur.h>
#include <KWayland/Client/compositor.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/region.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>
#include <wayland-client.h>

using namespace KWayland::Client;

WaylandBlurManager *WaylandBlurManager::instance()
{
    static WaylandBlurManager *self = new WaylandBlurManager(qApp);
    return self;
}

WaylandBlurManager::WaylandBlurManager(QObject *parent)
    : QObject(parent)
{
    if (!QGuiApplication::platformName().contains(QLatin1String("wayland")))
        return;

    // A compositor whose blur effect is broken paints black where the blurred
    // backdrop belongs, and every translucent window turns into a dark slab.
    // KWin does exactly that on virgl, so leave a way out that does not mean
    // rebuilding: FISHUI_DISABLE_BLUR=1 in the session environment.
    if (qEnvironmentVariableIntValue("FISHUI_DISABLE_BLUR") != 0)
        return;

    ConnectionThread *connection = ConnectionThread::fromApplication(this);
    if (!connection)
        return;

    m_compositor = Compositor::fromApplication(this);

    Registry *registry = new Registry(this);
    registry->create(connection);

    // Legacy KWin 5 interface
    connect(registry, &Registry::blurAnnounced, this, [this, registry](quint32 name, quint32 version) {
        m_manager = registry->createBlurManager(name, version, this);
        Q_EMIT ready();
    });

    // Modern standard Wayland interface (KWin 6 / Plasma 6.2+)
    connect(registry, &Registry::interfaceAnnounced, this, [this, registry](const QByteArray &interface, quint32 name, quint32 version) {
        if (interface == "ext_background_effect_manager_v1") {
            m_extManager = static_cast<struct ext_background_effect_manager_v1 *>(
                wl_registry_bind(registry->registry(), name, &ext_background_effect_manager_v1_interface, qMin(version, 1u)));
            m_hasBlurCapability = true;

            static const struct ext_background_effect_manager_v1_listener s_listener = {
                [](void *data, struct ext_background_effect_manager_v1 *, uint32_t flags) {
                    auto *mgr = static_cast<WaylandBlurManager *>(data);
                    mgr->m_hasBlurCapability = (flags & EXT_BACKGROUND_EFFECT_MANAGER_V1_CAPABILITY_BLUR) != 0;
                    Q_EMIT mgr->ready();
                }
            };
            ext_background_effect_manager_v1_add_listener(m_extManager, &s_listener, this);
            Q_EMIT ready();
        }
    });

    registry->setup();

    // Roundtrip to ensure initial registry events and capabilities are received
    connection->roundtrip();
}

WaylandBlurManager::~WaylandBlurManager()
{
    for (auto it = m_extEffects.begin(); it != m_extEffects.end(); ++it) {
        if (it.value()) {
            ext_background_effect_surface_v1_destroy(it.value());
        }
    }
    m_extEffects.clear();

    qDeleteAll(m_legacyBlurs);
    m_legacyBlurs.clear();

    if (m_extManager) {
        ext_background_effect_manager_v1_destroy(m_extManager);
        m_extManager = nullptr;
    }
}

bool WaylandBlurManager::isValid() const
{
    const bool extValid = (m_extManager != nullptr && m_hasBlurCapability);
    const bool legacyValid = (m_manager != nullptr && m_manager->isValid());
    return (extValid || legacyValid) && (m_compositor != nullptr && m_compositor->isValid());
}

void WaylandBlurManager::onSurfaceDestroyed(QObject *obj)
{
    auto *surface = static_cast<Surface *>(obj);
    removeBlur(surface);
}

void WaylandBlurManager::updateBlur(Surface *surface, const QRegion &region)
{
    if (!isValid() || !surface || !surface->isValid())
        return;

    connect(surface, &QObject::destroyed, this, &WaylandBlurManager::onSurfaceDestroyed, Qt::UniqueConnection);

    if (m_extManager && m_hasBlurCapability) {
        struct ext_background_effect_surface_v1 *effect = m_extEffects.value(surface, nullptr);
        if (!effect) {
            struct wl_surface *wlSurf = *surface;
            if (!wlSurf)
                return;
            effect = ext_background_effect_manager_v1_get_background_effect(m_extManager, wlSurf);
            if (!effect)
                return;
            m_extEffects.insert(surface, effect);
        }

        if (region.isEmpty()) {
            ext_background_effect_surface_v1_set_blur_region(effect, nullptr);
        } else {
            auto kregion = m_compositor->createRegion(region);
            if (kregion && kregion->isValid()) {
                ext_background_effect_surface_v1_set_blur_region(effect, *kregion);
            }
        }
    } else if (m_manager && m_manager->isValid()) {
        Blur *blur = m_legacyBlurs.value(surface, nullptr);
        if (!blur) {
            blur = m_manager->createBlur(surface, surface);
            if (!blur)
                return;
            m_legacyBlurs.insert(surface, blur);
        }

        if (region.isEmpty()) {
            blur->setRegion(nullptr);
            blur->commit();
        } else {
            auto kregion = m_compositor->createRegion(region);
            if (kregion && kregion->isValid()) {
                blur->setRegion(kregion.get());
                blur->commit();
            }
        }
    }
}

void WaylandBlurManager::removeBlur(Surface *surface)
{
    if (!surface)
        return;

    if (m_extEffects.contains(surface)) {
        struct ext_background_effect_surface_v1 *effect = m_extEffects.take(surface);
        if (effect) {
            ext_background_effect_surface_v1_destroy(effect);
        }
    }

    if (m_legacyBlurs.contains(surface)) {
        Blur *blur = m_legacyBlurs.take(surface);
        delete blur;
        if (m_manager && m_manager->isValid()) {
            m_manager->removeBlur(surface);
        }
    }
}

std::unique_ptr<Region> WaylandBlurManager::createRegion(const QRegion &region)
{
    if (!m_compositor || !m_compositor->isValid())
        return nullptr;

    return m_compositor->createRegion(region);
}
