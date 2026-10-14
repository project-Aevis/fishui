#include "iconthemeprovider.h"
#include <QFile>
#include <QIcon>
#include <QPixmapCache>

IconThemeProvider::IconThemeProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap IconThemeProvider::requestPixmap(const QString &id, QSize *realSize,
                                         const QSize &requestedSize)
{
    // Sanitize requested size: default to 128 if not specified
    QSize size(requestedSize);
    if (size.width() < 1)
        size.setWidth(128);
    if (size.height() < 1)
        size.setHeight(128);

    // Return real size
    if (realSize)
        *realSize = size;

    const QString cacheKey = QStringLiteral("cf_icontheme_%1_%2x%3")
        .arg(id).arg(size.width()).arg(size.height());

    QPixmap cachedPix;
    if (QPixmapCache::find(cacheKey, &cachedPix)) {
        return cachedPix;
    }

    if (QFile::exists(id) || id.startsWith(QLatin1Char('/'))) {
        if (id.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)
            || id.endsWith(QLatin1String(".svgz"), Qt::CaseInsensitive)) {
            QIcon svgIcon(id);
            if (!svgIcon.isNull()) {
                cachedPix = svgIcon.pixmap(size);
                QPixmapCache::insert(cacheKey, cachedPix);
                return cachedPix;
            }
        }
        QPixmap pix(id);
        if (!pix.isNull()) {
            cachedPix = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QPixmapCache::insert(cacheKey, cachedPix);
            return cachedPix;
        }
    }

    // Return icon from theme or fallback to a generic icon
    QIcon icon = QIcon::fromTheme(id);
    if (icon.isNull()) {
        const QString cur = QIcon::themeName();
        if (cur != QLatin1String("Cutefish")) {
            QIcon::setThemeName(QLatin1String("Cutefish"));
            icon = QIcon::fromTheme(id);
            QIcon::setThemeName(cur);
        }
    }
    if (icon.isNull())
        icon = QIcon::fromTheme(QLatin1String("application-x-desktop"));

    cachedPix = icon.pixmap(size);
    QPixmapCache::insert(cacheKey, cachedPix);
    return cachedPix;
}
