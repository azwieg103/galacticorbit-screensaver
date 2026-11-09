# RPM packaging (AlmaLinux 10)

The RPM includes the SDL3/OpenGL preview application, planetary texture maps,
and the KDE Plasma 6 wallpaper plugin. The plugin can be selected specifically
as the **KScreenLocker background**; it does not replace the secure lock-screen
shell or password authentication.

## Install build dependencies

```bash
sudo dnf install -y epel-release dnf-plugins-core
sudo dnf config-manager --set-enabled crb
sudo dnf install -y rpm-build rpmdevtools cmake gcc-c++ make SDL3-devel \
  mesa-libGL-devel extra-cmake-modules libplasma-devel kf6-kcoreaddons-devel \
  qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtshadertools-devel curl
```

## Build from the repository root

```bash
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
git archive --format=tar.gz --prefix=galacticorbit-screensaver-0.1.0/ HEAD \
  > ~/rpmbuild/SOURCES/galacticorbit-screensaver-0.1.0.tar.gz
cp packaging/galacticorbit-screensaver.spec ~/rpmbuild/SPECS/
rpmbuild -ba ~/rpmbuild/SPECS/galacticorbit-screensaver.spec
```

## Install the RPM

```bash
sudo dnf install ~/rpmbuild/RPMS/x86_64/galacticorbit-screensaver-*.rpm
```

## Select it for the lock screen

1. Open **System Settings → Security & Privacy → Screen Locking** (the
   category wording may vary slightly by Plasma version).
2. Find **Appearance** and click **Configure Appearance…**.
3. Under **Wallpaper Type**, select **Galactic Orbit**.
4. Apply the change and press **Meta+L** to test.

Do not select it from the desktop's Wallpaper settings; the desktop wallpaper
and lock-screen background have independent configurations. KDE's normal
lock-screen controls and password authentication remain enabled.

The plugin is installed under
`/usr/share/plasma/wallpapers/org.zwiegnet.galacticorbit`, where Plasma's
wallpaper package loader discovers it. To inspect installed wallpaper packages:

```bash
kpackagetool6 --type Plasma/Wallpaper --list
```

The standalone preview can be launched with
`/usr/bin/galacticorbit-screensaver --windowed`.
