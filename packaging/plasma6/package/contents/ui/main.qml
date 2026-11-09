import QtQuick
import org.kde.plasma.plasmoid
import org.zwiegnet.galacticorbit.geometry 1.0

WallpaperItem {
    id: root
    property rect screenRect: Qt.rect(0, 0, width, height)
    property rect desktopRect: Qt.rect(0, 0, width, height)
    property real rotationSpeed: configuration.rotationSpeed / 100.0
    property bool showLabels: configuration.showLabels
    function refreshGeometry() {
        const s = DisplayGeometry.screenGeometry(root)
        const d = DisplayGeometry.virtualGeometry(root)
        if (s.width > 0 && s.height > 0) screenRect = s
        if (d.width > 0 && d.height > 0) desktopRect = d
    }
    Component.onCompleted: refreshGeometry()
    Timer { interval: 1500; repeat: true; running: true; onTriggered: root.refreshGeometry() }
    FrameAnimation {
        running: true
        onTriggered: shader.phase = DisplayGeometry.elapsedSeconds()
    }
    ShaderEffect {
        id: shader
        anchors.fill: parent
        property real phase: DisplayGeometry.elapsedSeconds()
        property vector2d desktopSize: Qt.vector2d(Math.max(1, root.desktopRect.width), Math.max(1, root.desktopRect.height))
        property vector2d screenOrigin: Qt.vector2d(root.screenRect.x - root.desktopRect.x, root.screenRect.y - root.desktopRect.y)
        property vector2d screenSize: Qt.vector2d(root.width, root.height)
        property real rotationSpeed: root.rotationSpeed
        property real showLabels: root.showLabels ? 1.0 : 0.0
        fragmentShader: "qrc:/org/zwiegnet/galacticorbit/shaders/galaxy.frag.qsb"
    }
}
