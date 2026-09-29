#ifndef WAYLANDBLURMANAGER_H
#define WAYLANDBLURMANAGER_H

#include <QObject>
#include <QRegion>
#include <QHash>
#include <memory>

struct ext_background_effect_manager_v1;
struct ext_background_effect_surface_v1;

namespace KWayland
{
namespace Client
{
class Blur;
class BlurManager;
class Compositor;
class Region;
class Surface;
}
}

/**
 * Process-wide access to compositor background blur.
 *
 * Supports modern Wayland staging protocol ext_background_effect_v1 (KWin 6 / Plasma 6.2+)
 * and legacy org_kde_kwin_blur (KWin 5).
 */
class WaylandBlurManager : public QObject
{
    Q_OBJECT

public:
    static WaylandBlurManager *instance();

    //! Whether a blur interface is available from the compositor.
    bool isValid() const;

    //! Applies or updates the blur region for @p surface.
    void updateBlur(KWayland::Client::Surface *surface, const QRegion &region);

    //! Removes blur for @p surface and frees associated compositor effect resources.
    void removeBlur(KWayland::Client::Surface *surface);

    //! Wraps @p region for the protocol.
    std::unique_ptr<KWayland::Client::Region> createRegion(const QRegion &region);

Q_SIGNALS:
    //! Emitted when a blur interface becomes available.
    void ready();

private Q_SLOTS:
    void onSurfaceDestroyed(QObject *obj);

private:
    explicit WaylandBlurManager(QObject *parent = nullptr);
    ~WaylandBlurManager() override;

    KWayland::Client::BlurManager *m_manager = nullptr;
    KWayland::Client::Compositor *m_compositor = nullptr;
    struct ext_background_effect_manager_v1 *m_extManager = nullptr;
    bool m_hasBlurCapability = false;

    // Track active blur per surface for both backends
    QHash<KWayland::Client::Surface *, KWayland::Client::Blur *> m_legacyBlurs;
    QHash<KWayland::Client::Surface *, struct ext_background_effect_surface_v1 *> m_extEffects;
};

#endif // WAYLANDBLURMANAGER_H
