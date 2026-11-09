# RPM packaging (AlmaLinux 10)

The RPM includes the SDL3/OpenGL preview application, locally installed planetary
texture maps, and the KDE Plasma 6 wallpaper package. The Plasma wallpaper uses
the virtual desktop geometry and a shared monotonic animation clock so each
monitor renders its own slice of one continuous scene.

Install build dependencies:

```bash
sudo dnf install -y rpm-build rpmdevtools dnf-plugins-core
sudo dnf install -y epel-release
sudo dnf config-manager --set-enabled crb
sudo dnf install -y cmake gcc-c++ make SDL3-devel mesa-libGL-devel \
  extra-cmake-modules libplasma-devel kf6-kcoreaddons-devel \
  qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtshadertools-devel
```

Build from the repository root:

```bash
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
git archive --format=tar.gz --prefix=galacticorbit-screensaver-0.1.0/ HEAD \
  > ~/rpmbuild/SOURCES/galacticorbit-screensaver-0.1.0.tar.gz
cp packaging/galacticorbit-screensaver.spec ~/rpmbuild/SPECS/
rpmbuild -ba ~/rpmbuild/SPECS/galacticorbit-screensaver.spec
```

Install the resulting package:

```bash
sudo dnf install ~/rpmbuild/RPMS/x86_64/galacticorbit-screensaver-*.rpm
```

Then open **System Settings → Screen Locking**, select **Galactic Orbit** as
the wallpaper/background, and keep KDE's normal lock authentication enabled.
The standalone preview can be launched with
`/usr/bin/galacticorbit-screensaver --windowed`.

The KDE integration is a Plasma 6 wallpaper plugin; it is not a replacement
for the secure screen locker. On Wayland, the per-output Plasma wallpaper
instances are used instead of relying on one SDL window spanning outputs.
