# Galactic Orbit

<p align="center">
  <img src="docs/four-monitor-setup.svg" alt="Stylized illustration of Galactic Orbit running across a four-monitor workstation" width="100%">
</p>

<p align="center"><em>Four-monitor setup — a stylized illustration based on the real workstation installation.</em></p>

The scene is designed as one continuous view across all four displays, with the galactic center spanning the monitor junctions.

<details>
<summary>Conceptual view of the Milky Way scene</summary>
<p align="center">
  <img src="docs/galactic-orbit-overview.svg" alt="Conceptual top-down view of the Milky Way, Sagittarius A*, the Orion Spur, and the Solar System" width="100%">
</p>
<p><em>Schematic overview, not to scale and not a live renderer screenshot.</em></p>
</details>

Galactic Orbit is an animated Milky Way and Solar System visualization for Linux, with a KDE Plasma 6 wallpaper integration designed to work as a **KScreenLocker background**. The scene is intended to span a multi-monitor desktop as one continuous galactic canvas.

The project includes two renderers:

- **Plasma 6 integration:** a procedural OpenGL shader rendered as synchronized per-display slices, using shared virtual-desktop coordinates. The scene stays aligned across monitor boundaries.
- **Standalone preview:** an SDL3/OpenGL application with a texture-mapped Sun and planets. It can open a window on one display or a borderless window spanning the virtual desktop.

## Current visualization

The current Plasma scene includes:

- A broad, tilted, face-on Milky Way with a luminous central bulge, textured spiral structure, dust lanes, star fields, and blue-violet outer regions.
- **Sagittarius A*** at the galactic center, shown as a compact dark event horizon with a detailed, high-contrast accretion disk and photon ring.
- Five subtle labels identifying the **Norma–Outer Arm**, **Scutum–Centaurus Arm**, **Sagittarius–Carina Arm**, **Perseus Arm**, and **Orion Spur**. The labels follow the slow galactic rotation.
- The Sun positioned on the Orion Spur, with the planetary system orbiting around it.
- Eight planets with bundled surface textures and a shared orbital plane inclined 60° to the viewing direction. The circular paths project as ellipses in the top-down view.
- A two-line title, **“Milky Way Galaxy”** and **“Sagittarius A Black Hole,”** shown once in the upper-left display.

The shader layer is rendered at 45% of each display's physical width and height and smoothly upscaled to reduce GPU load; the text labels are drawn separately so they remain crisp. The Plasma lock-screen animation updates at approximately 15 FPS to reduce sustained GPU work. Every display samples the same monotonic clock, keeping the continuous scene synchronized despite independently timed updates.

The visualization is illustrative rather than a scale model. Galactic dimensions, distances, orbital periods, and animation rates are compressed or accelerated for viewing. The Milky Way's detailed spiral structure and pattern speed are not known with certainty.

## Automated builds and releases

GitHub Actions automatically builds and publishes the Enterprise Linux 10 RPM whenever code is pushed to `main`.

### Enterprise Linux RPM releases

Every successful build creates a new GitHub Release containing the binary RPM and source RPM. The package version follows the product version in `CMakeLists.txt`; the ZWIEGNET rebuild number increments for each published build of that product version.

For example:

```text
galacticorbit-screensaver-0.1.0-ZWIEGNET.1.el10.x86_64.rpm
galacticorbit-screensaver-0.1.0-ZWIEGNET.2.el10.x86_64.rpm
```

The RPM is built in an AlmaLinux 10 environment for x86_64 and is intended for AlmaLinux 10, Red Hat Enterprise Linux 10, Oracle Linux 10, Rocky Linux 10, and CentOS Stream 10. Runtime dependency availability should still be checked on each distribution. Failed builds do not publish a release, and previously published releases are not overwritten.

Packages are rebuilt from the source code in this repository and are supplied as-is, with no warranty or support. This is a community rebuild for Enterprise Linux, not an official upstream release.

### Cross-platform releases

Every successful push to `main` (and manual RPM workflow run) builds all supported targets. A release is published only after every platform build succeeds, so the downloadable packages represent the same source commit.

| Platform | Build artifact | Architecture |
|---|---|---|
| Enterprise Linux 10 | RPM and source RPM | x86_64 |
| Windows | Portable ZIP and MSI installer | x64 |
| Debian 13 | DEB package | amd64 |
| Ubuntu 26.04 | DEB package | amd64 |
| Arch Linux | Pacman package (`.pkg.tar.zst`) | x86_64 |
| FreeBSD 15.1 | Portable tar.gz package | amd64 |
| macOS | Portable ZIP package | Apple Silicon (arm64) |
| macOS | Portable ZIP package | Intel (x86_64) |

Windows packages include the SDL3 runtime DLL and bundled planetary textures. The macOS ZIPs include the SDL3 dynamic library and bundled textures. The FreeBSD archive contains the installed executable and data files; install the SDL3 and OpenGL runtime packages with `pkg` before launching it.

Debian and Ubuntu packages install the executable and planetary textures under standard system paths. Install them with:

```bash
sudo apt install ./galacticorbit-*.deb
```

Arch Linux packages can be installed with:

```bash
sudo pacman -U ./galacticorbit-screensaver-*.pkg.tar.zst
```

FreeBSD's portable archive can be unpacked with:

```sh
sudo tar -xzf galacticorbit-freebsd-15.1-amd64.tar.gz -C /
pkg install sdl3 mesa-libs
```

The macOS ZIP can be extracted in Finder; run `galacticorbit-screensaver` from the extracted directory.

## Install on AlmaLinux 10

The recommended installation is the RPM, which packages the standalone preview, bundled planetary textures, and KDE Plasma 6 wallpaper integration.

### Build dependencies

Enable the CRB repository and install the development packages:

```bash
sudo dnf install -y epel-release dnf-plugins-core
sudo dnf config-manager --set-enabled crb
sudo dnf install -y rpm-build rpmdevtools cmake gcc-c++ make SDL3-devel \
  mesa-libGL-devel extra-cmake-modules libplasma-devel kf6-kcoreaddons-devel \
  qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtshadertools-devel
```

### Build the RPM

Run these commands from the repository root:

```bash
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
cp packaging/galacticorbit-screensaver.spec ~/rpmbuild/SPECS/
git archive --format=tar.gz --prefix=galacticorbit-screensaver-0.1.0/ HEAD \
  > ~/rpmbuild/SOURCES/galacticorbit-screensaver-0.1.0.tar.gz
rpmbuild -ba ~/rpmbuild/SPECS/galacticorbit-screensaver.spec
```

Install the resulting RPM:

```bash
sudo dnf install ~/rpmbuild/RPMS/x86_64/galacticorbit-screensaver-*.rpm
```

For the full packaging notes, see [packaging/README.md](packaging/README.md).

## Select it as the KDE lock-screen background

1. Open **System Settings → Screen Locking**.
2. Open the **Appearance** section and choose **Configure Appearance…** if shown.
3. Select **Galactic Orbit** as the wallpaper/background.
4. Apply the setting and press **Meta+L** to test it.

The desktop wallpaper and lock-screen background are configured independently. Select Galactic Orbit in the lock-screen settings, not the ordinary desktop Wallpaper settings.

Galactic Orbit supplies only the background. KDE's normal lock-screen interface and password authentication remain in place; this project does not replace or bypass screen-lock security.

## Build from source

### Standalone SDL3/OpenGL preview

Install the standalone build dependencies:

```bash
sudo dnf install -y gcc-c++ cmake make SDL3-devel mesa-libGL-devel
```

Planetary texture maps are bundled in `assets/planet-textures/`; no download step is required for a normal build.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run the preview:

```bash
./build/galacticorbit-screensaver
```

The default mode opens a borderless window spanning the virtual desktop. Other useful options:

| Command | Purpose |
|---|---|
| `./build/galacticorbit-screensaver --windowed` | Open a 1280×720 window on one display |
| `./build/galacticorbit-screensaver --windowed-span` | Open a borderless window spanning the virtual desktop |
| `./build/galacticorbit-screensaver --speed 1.5` | Start with a faster animation rate |
| `./build/galacticorbit-screensaver --help` | Show command-line usage |

On Wayland, compositor behavior can affect how a single SDL window spans multiple outputs. The Plasma integration is the preferred multi-monitor implementation because each output renders its own synchronized slice.

### KDE Plasma 6 integration

Install the Plasma development packages listed in the RPM build dependencies, then configure and build with the Plasma integration enabled:

```bash
cmake -S . -B build-plasma -DCMAKE_BUILD_TYPE=Release \
  -DGALACTICORBIT_BUILD_PLASMA6=ON
cmake --build build-plasma --parallel
sudo cmake --install build-plasma
```

For normal use, prefer installing the RPM so the wallpaper package, geometry plugin, shader resources, and texture assets are installed in their expected system locations.

## Standalone preview controls

| Key | Action |
|---|---|
| `Esc` or `Q` | Quit |
| `Space` | Pause or resume |
| `R` | Reset animation time |
| `+` / `-` | Increase / decrease animation speed |
| `F` | Toggle fullscreen |

## Project layout

- `src/` — standalone SDL3/OpenGL renderer.
- `assets/planet-textures/` — bundled Solar System texture maps.
- `packaging/plasma6/` — KDE Plasma 6 wallpaper package, geometry helper, and procedural fragment shader.
- `packaging/galacticorbit-screensaver.spec` — Enterprise Linux RPM specification.
- `packaging/arch/PKGBUILD` — Arch Linux package recipe.
- `.github/workflows/` — automated cross-platform build and release workflows.

## License

The application is licensed under **GNU GPL-3.0-only**; see [LICENSE](LICENSE). Planetary texture maps have their own upstream attribution and license terms; consult the included asset/source notes before redistributing those files.
