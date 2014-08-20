import QtQuick 2.0
import org.nemomobile.lipstick 0.1
import org.nemomobile.dbus 1.0
import org.nemomobile.configuration 1.0
import com.jolla.lipstick 0.1
import Sailfish.Silica 1.0
import org.coderus.desktopfilemodel 1.0

SystemWindow {
    id: powerMenuDialog
    anchors.fill: parent

    opacity: 0
    enabled: opacity > 0

    property Item selectedItem
    property Item remorse

    property bool screenIsLocked: desktop.screenIsLocked
    onScreenIsLockedChanged: {
        if (screenIsLocked && powerMenuDialog.enabled) {
            hideDialog()
        }
    }

    property bool deviceIsLocked: desktop.deviceIsLocked
    onDeviceIsLockedChanged: {
        if (deviceIsLocked && powerMenuDialog.enabled) {
            hideDialog()
        }
    }

    Component.onCompleted: {
        console.log("PowerMenuDialog Loaded!")
        powermenuDbus.call("ready", [])

        if (configurationPowermenu.fancyBackground) {
            var shaderComponent = Qt.createComponent("/usr/share/powermenu-gui/qmls/ShaderTiledBackground.qml");
            var shaderObject = shaderComponent.createObject(powerMenuDialogBackground, {"color": Theme.primaryColor});
        }
    }

    function showDialog() {
        dialogAdaptor.emitSignal("dialogOpened")
        powerMenuDialog.opacity = 1.0

        topPeekAllowed = false
        leftPeekAllowed = false
        rightPeekAllowed = false
        bottomPeekAllowed = false
    }

    function hideDialog() {
        dialogAdaptor.emitSignal("dialogHidden")
        powerMenuDialog.opacity = 0.0

        topPeekAllowed = true
        leftPeekAllowed = true
        rightPeekAllowed = true
        bottomPeekAllowed = true
    }

    function restart() {
        dsmeDbus.call("req_reboot", [])
        hideDialog()
    }

    function shutdown() {
        dsmeDbus.call("req_shutdown", [])
        hideDialog()
    }

    function remorseRestart() {
        if (!remorse) {
            remorse = remorseComponent.createObject(powerMenuDialog)
        }

        remorse.execute("Reboot phone",
                        function() {
                            powerMenuDialog.restart()
                        }
        )
    }

    function remorseShutdown() {
        if (!remorse) {
            remorse = remorseComponent.createObject(powerMenuDialog)
        }

        remorse.execute("Poweroff phone",
                        function() {
                            powerMenuDialog.shutdown()
                        }
        )
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightDimmerColor, 0.8)
    }

    Rectangle {
        id: powerMenuDialogBackground

        width: parent.width
        height: column.height + column.y
        color: Theme.highlightBackgroundColor

        /*Image {
            id: patternBackground
            source: "/usr/share/powermenu-gui/images/gear-pattern.png"
            anchors.fill: parent
            fillMode: Image.Tile
            visible: configurationPowermenu.fancyBackground
            layer.effect: ShaderEffect {
                id: shaderItem
                property color color: Theme.primaryColor

                fragmentShader: "
                    varying mediump vec2 qt_TexCoord0;
                    uniform highp float qt_Opacity;
                    uniform lowp sampler2D source;
                    uniform highp vec4 color;
                    void main() {
                        highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                        gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                    }
                "
            }
            layer.enabled: true
            layer.samplerName: "source"
        }*/

        Column {
            id: column

            width: parent.width
            y: Theme.paddingLarge
            z: 1
            Label {
                text: "Power Menu"
                maximumLineCount: 2
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }

                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba("black", 0.6)
            }
            Row {
                id: buttonRow
                property real buttonWidth: width / 2
                width: parent.width
                visible: configurationPowermenu.showShutdown

                SystemDialogButton {
                    width: buttonRow.buttonWidth
                    text: "Shutdown"
                    iconSource: "image://theme/icon-l-power?#000000"
                    onClicked: {
                        powerMenuDialog.remorseShutdown()
                    }
                }

                SystemDialogButton {
                    width: buttonRow.buttonWidth
                    text: "Reboot"
                    iconSource: "image://theme/icon-l-backup?#000000"
                    onClicked: {
                        powerMenuDialog.remorseRestart()
                    }
                }
            }

            Repeater {
                id: desktopRepeater
                width: parent.width
                model: desktopModel
                delegate: BackgroundItem {
                    id: item
                    width: parent.width
                    height: 100
                    highlightedColor: Theme.primaryColor

                    Image {
                        id: iconImage
                        source: model.icon
                        width: 80
                        height: 80
                        smooth: true
                        anchors {
                            left: parent.left
                            leftMargin: Theme.paddingLarge
                            verticalCenter: parent.verticalCenter
                        }
                    }

                    Label {
                        text: model.name
                        anchors {
                            left: iconImage.right
                            leftMargin: Theme.paddingMedium
                            verticalCenter: parent.verticalCenter
                        }
                        color: item.highlighted ? Theme.highlightColor : Theme.rgba("black", 0.4)
                    }

                    onClicked: {
                        hideDialog()
                        powermenuDbus.call("openDesktop", [model.path])
                    }
                }
            }
        }
    }

    MouseArea {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: powerMenuDialogBackground.bottom
        }

        onClicked: {
            hideDialog()
        }

        Image {
            width: parent.width
            source: "image://theme/graphic-system-gradient?" + Theme.highlightBackgroundColor
        }
    }

    Behavior on opacity { FadeAnimation {} }

    DBusInterface {
        id: dsmeDbus
        busType: DBusInterface.SystemBus
        destination: "com.nokia.dsme"
        path: "/com/nokia/dsme/request"
        iface: "com.nokia.dsme.request"
    }

    DBusInterface {
        id: powermenuDbus
        busType: DBusInterface.SessionBus
        destination: "org.coderus.powermenu"
        path: "/"
        iface: "org.coderus.powermenu"
    }

    DBusAdaptor {
        id: dialogAdaptor
        service: "com.jolla.lipstick.PowerMenuDialog"
        path: "/org/coderus/powermenu"
        iface: "com.jolla.lipstick.PowerMenuDialogIf"
        xml: "\t<interface name=\"com.jolla.lipstick.PowerMenuDialogIf\">\n" +
             "\t\t<method name=\"openDialog\">\n" +
             "\t\t</method>\n" +
             "\t\t<signal name=\"dialogOpened\">\n" +
             "\t\t</signal>\n" +
             "\t\t<signal name=\"dialogHidden\">\n" +
             "\t\t</signal>\n" +
             "\t</interface>\n"

        signal openDialog
        onOpenDialog: {
            if (!screenIsLocked && !deviceIsLocked) {
                showDialog()
            }
        }
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: configurationPowermenu.shortcuts
        onlySelected: true
        showHidden: false
    }

    ConfigurationGroup {
        id: configurationPowermenu
        path: "/apps/powermenu"
        property bool showShutdown: true
        property variant shortcuts
        property bool fancyBackground: true
    }

    Component {
        id: remorseComponent

        RemorsePopup {
            z: 100
        }
    }
}
