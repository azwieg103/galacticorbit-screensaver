Name:           galacticorbit-screensaver
Version:        0.1.0
Release:        18%{?dist}
Summary:        Animated Milky Way background for KDE Plasma 6 lock screen
License:        GPL-3.0-only
URL:            https://github.com/azwieg103/galacticorbit-screensaver
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake >= 3.20
BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  SDL3-devel
BuildRequires:  mesa-libGL-devel
BuildRequires:  extra-cmake-modules
BuildRequires:  libplasma-devel
BuildRequires:  kf6-kcoreaddons-devel
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtshadertools-devel

%description
An animated Milky Way and Solar System visualization for KDE Plasma 6.
The package installs a Plasma wallpaper plugin that can be selected
independently as the KScreenLocker background in System Settings ->
Screen Locking -> Appearance. KDE's normal lock screen and password
authentication remain unchanged. The package also includes a standalone
SDL3/OpenGL preview executable.

%prep
%autosetup

%build
%cmake -DCMAKE_BUILD_TYPE=Release -DGALACTICORBIT_BUILD_PLASMA6=ON
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%doc README.md
%{_bindir}/galacticorbit-screensaver

# Publicly traversable texture paths; greeter runs outside the user's session.
%attr(755,root,root) %dir %{_datadir}/galacticorbit
%attr(755,root,root) %dir %{_datadir}/galacticorbit/planet-textures
%attr(644,root,root) %{_datadir}/galacticorbit/planet-textures/*

# Plasma wallpaper package directories must be readable by the lock-screen greeter.
%attr(755,root,root) %dir %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit
%attr(755,root,root) %dir %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents
%attr(755,root,root) %dir %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/config
%attr(755,root,root) %dir %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/ui
%attr(644,root,root) %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/metadata.json
%attr(644,root,root) %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/config/main.xml
%attr(644,root,root) %{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/ui/main.qml

# Qt must be able to traverse the custom QML module path as the greeter user.
%attr(755,root,root) %dir %{_libdir}/qt6/qml/org/zwiegnet/galacticorbit
%attr(755,root,root) %dir %{_libdir}/qt6/qml/org/zwiegnet/galacticorbit/geometry
%attr(644,root,root) %{_libdir}/qt6/qml/org/zwiegnet/galacticorbit/geometry/qmldir
%attr(755,root,root) %{_libdir}/qt6/qml/org/zwiegnet/galacticorbit/geometry/libgalacticorbitgeometryplugin.so

%changelog
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-18
- Restore multi-octave cloud, dust, knot, and Orion Spur detail in the spiral arms.
- Retain the 45 percent shader layer and bounded planetary calculations for performance.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-17
- Reduce shader-layer resolution to 45 percent per dimension to lower sustained GPU load.
- Use single-octave secondary cloud fields and skip planetary loops outside the Solar System region.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-16
- Reverse the shader time phase so the spiral arms visibly travel clockwise on the screen.
- Keep the Orion Spur, Sun, and arm labels synchronized with the corrected motion.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-15
- Reverse the logarithmic spiral handedness so clockwise rotation also has clockwise arm curvature.
- Reorient the Orion Spur and keep its Sun and labels on the revised centerline.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-14
- Correct the rotation sign so the major spiral arms and Orion Spur rotate clockwise.
- Synchronize the Sun and spiral-arm labels with the corrected direction.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-13
- Reverse galactic pattern rotation so the spiral arms visibly rotate clockwise.
- Keep the Orion Spur, Sun, and arm labels synchronized with the corrected direction.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-12
- Restore the smaller Sagittarius A presentation and sharpen its accretion and photon rings.
- Remove the Sagittarius A* label and add a single two-line title on the upper-left display.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-11
- Add a crisp Sagittarius A* label near the galactic center.
- Enlarge and intensify the black hole, accretion disk, photon ring, and blue-violet halo.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-10
- Add crisp translucent labels for the four major spiral arms and the Orion Spur.
- Synchronize label anchors with the animated galactic rotation across all displays.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-9
- Replace individual planetary rings and trails with complete shared orbital-plane paths inclined 60 degrees.
- Keep the Sun locked to the Orion Spur centerline to prevent visible separation.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-8
- Increase Sagittarius A event horizon and surrounding rings to three times the original scale.
- Add thin planetary rings projected at a 60-degree inclination.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-7
- Replace diffuse planetary comet wakes with thin, defined orbital trail lines.
- Double the apparent diameter of the black hole and its surrounding rings.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-6
- Render the Plasma shader layer at half resolution with smooth upscaling.
- Reduce procedural noise octaves and reuse arm turbulence to lower fragment cost.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-5
- Separate galaxy and planetary animation clocks for visible orbital motion.
- Increase spiral-arm pitch and width and make major arms less symmetric.
- Match Orion Spur pitch to the revised major-arm geometry.
* Wed Nov 11 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-4
- Bundle planet textures in the source tree and install them through CMake.
- Remove build-time network downloads and the curl build dependency.
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-3
- Set explicit root-owned Unix permissions for texture, Plasma wallpaper, and Qt QML directories.
- Ensure lock-screen greeter can traverse directories and read installed textures and QML files.
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-2
- Clarify and package Galactic Orbit as a selectable KScreenLocker background.
- Add explicit Plasma wallpaper package version metadata.
- Preserve KDE's standard lock screen and authentication UI.
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-1
- Add AlmaLinux 10 RPM packaging and KDE Plasma 6 wallpaper integration.
- Add synchronized per-display rendering over the virtual desktop.
