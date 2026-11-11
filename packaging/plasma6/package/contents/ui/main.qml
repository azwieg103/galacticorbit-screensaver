import QtQuick
import org.kde.plasma.plasmoid
import org.zwiegnet.galacticorbit.geometry 1.0

WallpaperItem {
    id: root
    property rect screenRect: Qt.rect(0, 0, width, height)
    property rect desktopRect: Qt.rect(0, 0, width, height)
    // Independent clocks: the galaxy pattern drifts slowly while planets visibly orbit.
    property real galaxyAnimationScale: 0.35
    property real planetAnimationScale: 0.65
    property real planetPhase: 0.0
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
        onTriggered: {
            const elapsed = DisplayGeometry.elapsedSeconds()
            shader.phase = elapsed * root.galaxyAnimationScale
            root.planetPhase = elapsed * root.planetAnimationScale
        }
    }
    Image { id: sunMap; source: "file:///usr/share/galacticorbit/planet-textures/sun.jpg"; visible: false; cache: true }
    Image { id: mercuryMap; source: "file:///usr/share/galacticorbit/planet-textures/mercury.jpg"; visible: false; cache: true }
    Image { id: venusMap; source: "file:///usr/share/galacticorbit/planet-textures/venus.jpg"; visible: false; cache: true }
    Image { id: earthMap; source: "file:///usr/share/galacticorbit/planet-textures/earth.jpg"; visible: false; cache: true }
    Image { id: marsMap; source: "file:///usr/share/galacticorbit/planet-textures/mars.jpg"; visible: false; cache: true }
    Image { id: jupiterMap; source: "file:///usr/share/galacticorbit/planet-textures/jupiter.jpg"; visible: false; cache: true }
    Image { id: saturnMap; source: "file:///usr/share/galacticorbit/planet-textures/saturn.jpg"; visible: false; cache: true }
    Image { id: uranusMap; source: "file:///usr/share/galacticorbit/planet-textures/uranus.jpg"; visible: false; cache: true }
    Image { id: neptuneMap; source: "file:///usr/share/galacticorbit/planet-textures/neptune.jpg"; visible: false; cache: true }
    ShaderEffect {
        id: shader
        anchors.fill: parent
        // Render the expensive procedural scene at half resolution, then let Qt
        // upscale the layer smoothly to the physical display resolution.
        layer.enabled: true
        layer.smooth: true
        layer.textureSize: Qt.size(
            Math.max(1, Math.ceil(root.width * 0.5)),
            Math.max(1, Math.ceil(root.height * 0.5))
        )
        property real phase: DisplayGeometry.elapsedSeconds() * root.galaxyAnimationScale
        property real planetPhase: root.planetPhase
        property vector2d desktopSize: Qt.vector2d(Math.max(1, root.desktopRect.width), Math.max(1, root.desktopRect.height))
        property vector2d screenOrigin: Qt.vector2d(root.screenRect.x - root.desktopRect.x, root.screenRect.y - root.desktopRect.y)
        property vector2d screenSize: Qt.vector2d(root.width, root.height)
        property real rotationSpeed: root.rotationSpeed
        property real showLabels: root.showLabels ? 1.0 : 0.0
        property var sunTex: sunMap
        property var mercuryTex: mercuryMap
        property var venusTex: venusMap
        property var earthTex: earthMap
        property var marsTex: marsMap
        property var jupiterTex: jupiterMap
        property var saturnTex: saturnMap
        property var uranusTex: uranusMap
        property var neptuneTex: neptuneMap
        fragmentShader: "qrc:/org/zwiegnet/galacticorbit/shaders/galaxy.frag.qsb"
    }
}
