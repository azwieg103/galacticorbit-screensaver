# Galactic Orbit Screensaver

A scientific-visualization-oriented Milky Way and Solar System display for Linux and KDE Plasma 6. The standalone SDL3/OpenGL preview renders a single scene over a borderless virtual-desktop window where the compositor permits it. The Plasma 6 wallpaper integration renders synchronized per-output slices using shared virtual-desktop coordinates, so the four-monitor wall is treated as one continuous canvas.

## Visualization goals

- Show the complete galactic disk and spiral structure with margins, not a zoomed-in arm fragment.
- Place Sagittarius A* at the center, with a deliberately magnified but clearly identified event-horizon/accretion-disk visualization.
- Represent the Orion Spur as a local branch in the disk.
- Keep spiral-pattern rotation separate from the Sun's galactic orbital motion. The Sun drifts relative to the pattern rather than being rigidly attached to an arm.
- Render the eight planets with locally downloaded Solar System Scope texture maps in the standalone renderer.

Astronomical timescales are accelerated for viewing. The global Milky Way arm geometry remains an observationally informed approximation; the Galaxy's exact spiral structure and pattern speed are not known with certainty. The Plasma wallpaper uses a procedural GPU visualization and is a distinct renderer from the texture-mapped SDL preview.

## Build on AlmaLinux 10

Install the standalone renderer dependencies:

```bash
sudo dnf install -y gcc-c++ cmake make SDL3-devel mesa-libGL-devel
```

Download the planetary maps and build:

```bash
bash scripts/download-planet-textures.sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/galacticorbit-screensaver --windowed
```

The standalone default launches a borderless virtual-desktop window. On Wayland, compositor rules can limit a single SDL window to one output; use the Plasma wallpaper integration for per-output coverage.

## Build the KDE Plasma 6 integration

Install the Plasma 6 development SDK packages (CRB and EPEL may be needed on AlmaLinux 10):

```bash
sudo dnf install -y epel-release dnf-plugins-core
sudo dnf config-manager --set-enabled crb
sudo dnf install -y extra-cmake-modules libplasma-devel kf6-kcoreaddons-devel \
  qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtshadertools-devel
cmake -S . -B build-plasma -DCMAKE_BUILD_TYPE=Release -DGALACTICORBIT_BUILD_PLASMA6=ON
cmake --build build-plasma --parallel
sudo cmake --install build-plasma
```

Select **System Settings → Screen Locking → Galactic Orbit**. Keep KDE's normal screen-lock authentication enabled; this package supplies a wallpaper, not a security mechanism.

## Build an AlmaLinux 10 RPM

See [packaging/README.md](packaging/README.md) for dependencies and commands. The RPM includes the SDL3 preview, planetary texture assets, and Plasma 6 wallpaper integration.

## Controls

Standalone preview: Esc/Q exits, Space pauses, R resets animation time, +/- changes speed, and F toggles fullscreen.

## License

GNU GPL-3.0. See [LICENSE](LICENSE). Planetary surface maps are distributed under their upstream CC BY 4.0 attribution terms; see the texture download/source notes.
