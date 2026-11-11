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
    property real galaxyPhase: 0.0
    property real rotationSpeed: configuration.rotationSpeed / 100.0
    property bool showLabels: configuration.showLabels

    function refreshGeometry() {
        const s = DisplayGeometry.screenGeometry(root)
        const d = DisplayGeometry.virtualGeometry(root)
        if (s.width > 0 && s.height > 0) screenRect = s
        if (d.width > 0 && d.height > 0) desktopRect = d
    }

    // Convert a point in the shader's galactic coordinate system into this
    // display's local pixel coordinates. Every display uses the same virtual
    // desktop geometry, so labels line up seamlessly across bezel boundaries.
    function galacticPoint(gx, gy) {
        const a = -0.20
        const ca = Math.cos(a)
        const sa = Math.sin(a)
        const vx = ca * gx - sa * gy
        const vy = sa * gx + ca * gy
        const px = vx * desktopRect.height + desktopRect.width * 0.5
        const py = (vy / 1.52) * desktopRect.height + desktopRect.height * 0.5
        return Qt.point(px - (screenRect.x - desktopRect.x),
                        py - (screenRect.y - desktopRect.y))
    }

    function armLabelPosition(index, radius) {
        const base = index * Math.PI * 0.5 + 0.52 + 0.11 * Math.sin(index * 2.17)
        const angle = base + Math.log(radius / 0.075) / 0.40
                      - galaxyPhase * 0.105 * rotationSpeed
        return galacticPoint(radius * Math.cos(angle), radius * Math.sin(angle))
    }

    function orionLabelPosition() {
        const radius = 0.405
        const angle = 2.35 + Math.log(radius / 0.34) / 0.40
                      - galaxyPhase * 0.105 * rotationSpeed
        return galacticPoint(radius * Math.cos(angle), radius * Math.sin(angle))
    }

    Component.onCompleted: refreshGeometry()
    Timer { interval: 1500; repeat: true; running: true; onTriggered: root.refreshGeometry() }
    FrameAnimation {
        running: true
        onTriggered: {
            const elapsed = DisplayGeometry.elapsedSeconds()
            root.galaxyPhase = elapsed * root.galaxyAnimationScale
            shader.phase = root.galaxyPhase
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
        property real phase: root.galaxyPhase
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

    // These labels are deliberately outside the half-resolution shader layer:
    // they remain crisp while their anchors track the rotating spiral arms.
    component ArmLabel: Text {
        required property int armIndex
        required property string label
        property real armRadius: 0.56
        readonly property point anchorPoint: root.armLabelPosition(armIndex, armRadius)
        x: anchorPoint.x - width * 0.5
        y: anchorPoint.y - height * 0.5
        text: label
        visible: root.showLabels && root.desktopRect.width > 0
        color: "#99d9eaff"
        style: Text.Outline
        styleColor: "#55365d91"
        font.family: "Sans Serif"
        font.pixelSize: Math.max(12, Math.min(18, root.height * 0.014))
        font.weight: Font.DemiBold
        font.letterSpacing: 1.8
        font.italic: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    ArmLabel { armIndex: 0; label: "NORMA–OUTER ARM" }
    ArmLabel { armIndex: 1; label: "SCUTUM–CENTAURUS ARM" }
    ArmLabel { armIndex: 2; label: "SAGITTARIUS–CARINA ARM" }
    ArmLabel { armIndex: 3; label: "PERSEUS ARM" }

    Text {
        id: sagittariusALabel
        readonly property point centerPoint: root.galacticPoint(0.0, 0.0)
        x: centerPoint.x - width * 0.5
        y: centerPoint.y - Math.max(42, root.height * 0.055) - height * 0.5
        text: "SAGITTARIUS A*"
        visible: root.showLabels
                 && centerPoint.x >= 0 && centerPoint.x <= root.width
                 && centerPoint.y >= 0 && centerPoint.y <= root.height
        color: "#c4e8f7ff"
        style: Text.Outline
        styleColor: "#aa172b55"
        font.family: "Sans Serif"
        font.pixelSize: Math.max(16, Math.min(24, root.height * 0.019))
        font.weight: Font.Bold
        font.letterSpacing: 3.0
        font.italic: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Text {
        id: orionLabel
        readonly property point anchorPoint: root.orionLabelPosition()
        x: anchorPoint.x - width * 0.5
        y: anchorPoint.y - height * 0.5
        text: "ORION SPUR"
        visible: root.showLabels && root.desktopRect.width > 0
        color: "#a8e3f4ff"
        style: Text.Outline
        styleColor: "#663b719f"
        font.family: "Sans Serif"
        font.pixelSize: Math.max(12, Math.min(18, root.height * 0.014))
        font.weight: Font.DemiBold
        font.letterSpacing: 2.2
        font.italic: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
