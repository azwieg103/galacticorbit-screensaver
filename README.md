# Galactic Orbit Screensaver

A native Linux/OpenGL screensaver prototype for a four-monitor desktop. It renders one continuous face-on galaxy across the combined desktop, with the Sun traveling along a glowing orbital path through the Orion Arm and planets orbiting the Sun.

## Visual design

- One continuous galaxy scene, not four duplicated images.
- The galactic center remains the visual anchor.
- The Orion Arm rotates slowly clockwise.
- The Sun moves around the galactic center on a persistent glowing trail.
- Planets orbit the Sun at distinct speeds and distances; their motion is emphasized over the slow galactic rotation.
- Scene geometry stays within a safe margin of the drawable desktop area.

Timescales are artistically accelerated, not physically scaled.

## Status

Initial prototype. Multi-monitor spanning depends on the desktop compositor. X11 generally permits a borderless window over the virtual desktop. Wayland compositors may constrain fullscreen windows to one output; test the borderless spanning mode with KDE.

## Build on AlmaLinux / Fedora

Install dependencies:

```bash
sudo dnf install gcc-c++ cmake make SDL3-devel mesa-libGL-devel
```

Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Try a normal window first:

```bash
./build/galacticorbit-screensaver --windowed
```

Try a borderless window spanning the combined desktop:

```bash
./build/galacticorbit-screensaver --windowed-span
```

Run fullscreen:

```bash
./build/galacticorbit-screensaver
```

Press Esc or Q to exit. Press F to toggle fullscreen, Space to pause, R to reset animation time, and +/- to change speed.

## License

GNU General Public License v3.0 (GPL-3.0). See [LICENSE](LICENSE).
