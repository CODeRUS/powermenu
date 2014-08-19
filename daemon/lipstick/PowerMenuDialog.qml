import QtQuick 2.0
import org.nemomobile.lipstick 0.1
import org.nemomobile.dbus 1.0
import org.nemomobile.configuration 1.0
import com.jolla.lipstick 0.1
import Sailfish.Silica 1.0
import org.coderus.desktopfilemodel 1.0

import "scripts/desktop.js" as Desktop

SystemWindow {
    id: powerMenuDialog
    anchors.fill: parent

    opacity: 0
    enabled: opacity > 0

    property Item selectedItem
    property Item remorse

    Component.onCompleted: {
        console.log("PowerMenuDialog Loaded!")
        powermenuDbus.call("ready", [])
    }

    function showDialog() {
        dialogAdaptor.emitSignal("dialogOpened")
        powerMenuDialog.opacity = 1.0
    }

    function hideDialog() {
        dialogAdaptor.emitSignal("dialogHidden")
        powerMenuDialog.opacity = 0.0
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
        Column {
            id: column

            width: parent.width
            y: Theme.paddingLarge
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
                visible: configurationShowShutdown.value

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
            if (!Desktop.instance.screenIsLocked && !Desktop.instance.deviceIsLocked) {
                showDialog()
            }
        }
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: configurationShortcuts.value
        onlySelected: true
    }

    ConfigurationValue {
        id: configurationShowShutdown
        key: "/apps/powermenu/showShutdown"
        defaultValue: true
    }

    ConfigurationValue {
        id: configurationShortcuts
        key: "/apps/powermenu/shortcuts"
        defaultValue: []
    }

    Component {
        id: remorseComponent

        RemorsePopup {
            z: 100
        }
    }
}
