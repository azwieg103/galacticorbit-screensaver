Name:           galacticorbit-screensaver
Version:        0.1.0
Release:        3%{?dist}
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
BuildRequires:  curl

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
bash scripts/download-planet-textures.sh
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
- Improve galaxy framing, black-hole visibility, and solar-system placement.
