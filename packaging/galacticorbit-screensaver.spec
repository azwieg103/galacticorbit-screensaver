Name:           galacticorbit-screensaver
Version:        0.1.0
Release:        1%{?dist}
Summary:        Milky Way and Solar System screensaver for KDE Plasma 6
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
A multi-monitor Milky Way and Solar System visualization using SDL3/OpenGL.
The package includes a KDE Plasma 6 wallpaper integration that renders one
continuous, synchronized scene across the virtual desktop.

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
* Mon Nov 09 2026 Zwiegnet <programming@zwiegnet.com> - 0.1.0-1
- Add AlmaLinux 10 RPM packaging and KDE Plasma 6 wallpaper integration.
- Add synchronized per-display rendering over the virtual desktop.
- Improve galaxy framing, black-hole visibility, and independent Sun motion.
