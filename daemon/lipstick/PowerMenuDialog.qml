import QtQuick 2.0
import org.nemomobile.lipstick 0.1
import org.nemomobile.dbus 1.0
import org.nemomobile.configuration 1.0
import com.jolla.lipstick 0.1
import Sailfish.Silica 1.0
import org.coderus.desktopfilemodel 1.0
import "systemwindow"

SystemWindow {
    id: powerMenuDialog

    opacity: 0
    enabled: opacity > 0

    property Item selectedItem
    property Item remorse
    property Item tiledBackground

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

        var shaderComponent = Qt.createComponent("/usr/share/powermenu-gui/qmls/ShaderTiledBackground.qml");
        tiledBackground = shaderComponent.createObject(powerMenuDialogBackground, {"color": Theme.primaryColor, "visible": configurationPowermenu.fancyBackground});
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

        remorse.execute("Shutdown phone",
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

        property int maximumHeight: powerMenuDialog.height - flow.y - (transpose && configurationPowermenu.showShutdown ? (buttonRow.y + buttonRow.height) : 0) - Theme.itemSizeMedium
        width: parent.width
        height: flow.height + flow.y
        color: Theme.highlightBackgroundColor

        Flow {
            id: flow
            width: parent.width
            y: Theme.paddingLarge
            spacing: 0
            z: 1

            Label {
                text: "Power Menu"
                maximumLineCount: 2
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width

                font.pixelSize: Theme.fontSizeLarge
                color: Theme.rgba("black", 0.6)
            }

            Row {
                id: buttonRow
                property real buttonWidth: width / 2
                width: transpose ? parent.width : (parent.width / 2)
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

            ListView {
                width: transpose ? parent.width : (parent.width / 2)
                height: Math.min(contentHeight, powerMenuDialogBackground.maximumHeight)
                model: desktopModel
                interactive: contentHeight > height
                clip: true
                delegate: BackgroundItem {
                    id: item
                    width: parent.width
                    height: 100
                    highlightedColor: Theme.rgba(Theme.highlightDimmerColor, Theme.highlightBackgroundOpacity)

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
                        color: item.highlighted ? Theme.highlightColor : Theme.rgba(Theme.highlightDimmerColor, 0.8)
                    }

                    onClicked: {
                        hideDialog()
                        powermenuDbus.call("openDesktop", [model.path])
                    }
                }
                VerticalScrollDecorator {}
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
        xml: "<interface name=\"com.jolla.lipstick.PowerMenuDialogIf\">\n" +
             "\t<method name=\"openDialog\">\n" +
             "\t\t<annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>\n" +
             "\t</method>\n" +
             "\t<signal name=\"dialogOpened\">\n" +
             "\t</signal>\n" +
             "\t<signal name=\"dialogHidden\">\n" +
             "\t</signal>\n" +
             "</interface>\n"

        function openDialog() {
            if (!screenIsLocked && !deviceIsLocked) {
                showDialog()
            }
        }
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: configurationPowermenu.shortcuts
        onlySelected: true
        showHidden: true
    }

    ConfigurationGroup {
        id: configurationPowermenu
        path: "/apps/powermenu"
        property bool showShutdown: true
        property variant shortcuts
        property bool fancyBackground: true
        onFancyBackgroundChanged: tiledBackground.visible = fancyBackground
    }

    Component {
        id: remorseComponent

        RemorsePopup {
            z: 100
        }
    }
}
