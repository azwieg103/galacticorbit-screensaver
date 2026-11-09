Name:           galacticorbit-screensaver
Version:        0.1.0
Release:        2%{?dist}
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
%{_datadir}/galacticorbit/planet-textures/*
%{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/metadata.json
%{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/config/main.xml
%{_datadir}/plasma/wallpapers/org.zwiegnet.galacticorbit/contents/ui/main.qml
%{_libdir}/qt6/qml/org/zwiegnet/galacticorbit/geometry/qmldir
%{_libdir}/qt6/qml/org/zwiegnet/galacticorbit/geometry/libgalacticorbitgeometryplugin.so

%changelog
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-2
- Clarify and package Galactic Orbit as a selectable KScreenLocker background.
- Add explicit Plasma wallpaper package version metadata.
- Preserve KDE's standard lock screen and authentication UI.
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-1
- Add AlmaLinux 10 RPM packaging and KDE Plasma 6 wallpaper integration.
- Add synchronized per-display rendering over the virtual desktop.
- Improve galaxy framing, black-hole visibility, and solar-system placement.
