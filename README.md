# FishUI

FishUI is the core QML design system and component library built on Qt Quick Controls 2 (QQC2) for the Aevis Desktop Environment.

Part of the **[Project Aevis](https://github.com/project-Aevis)** suite.

## Features

- **Light and Dark Themes**: Automatic system color scheme detection and manual toggle.
- **Wayland Window Management**: Borderless window move, resize, and custom title bars.
- **Visual Effects**: Native blurred windows, drop shadows, and acrylic transparency effects.
- **Desktop Controls**: Desktop-level menus, toolbars, sidebars, and customizable QQC2 controls.

## Dependencies

### Debian / Ubuntu

```shell
sudo apt install cmake extra-cmake-modules build-essential \
    qt6-base-dev qt6-base-private-dev qt6-declarative-dev
```

### Arch Linux

```shell
sudo pacman -S cmake extra-cmake-modules qt6-base qt6-declarative
```

## Build and Install

```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

## Packaging (Debian / Ubuntu)

```shell
sudo apt install equivs devscripts --no-install-recommends
sudo mk-build-deps -i -t "apt-get --yes" -r
dpkg-buildpackage -b -uc -us
```

## License

FishUI is licensed under the GNU General Public License v3.0 (GPLv3).
