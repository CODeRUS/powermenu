import QtQuick 2.1
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0
import ".."

Page {
    id: page
    objectName: "configurationPage"

    onStatusChanged: {
        if (status == PageStatus.Active && helper.shouldRestartLipstick) {
            pullDown.active = true
            flick.contentY = -160
        }
    }

    // Valid options:
    // disabled - do nothing on short press
    // poweroff - shutdown device
    // softpoweroff - enter soft poweroff mode
    // tklock-lock - lock touchscreen/keypad lock if not locked
    // tklock-unlock - unlock the touchscreen/keypad lock if locked
    // tklock-both - lock the touchscreen/keypad if not locked,
    //               unlock the touchscreen/keypad lock if locked
    // dbus-signal-<signal name> - send a D-Bus signal with the name <signal name> (powerkeyLong, powerkeyDouble, powerkeyShort)

    function getActionIndex(action) {
        if (action === "disabled")
            return 0
        else if (action === "poweroff")
            return 1
        else if (action === "softpoweroff")
            return 2
        else if (action === "tklock-lock")
            return 3
        else if (action === "tklock-unlock")
            return 4
        else if (action === "tklock-both")
            return 5
        else if (action === "dbus-signal-powerkeyMenu")
            return 6
        else if (action.indexOf("dbus-signal-action") == 0)
            return 7
        else
            console.log("unknown action: " + action)
            return -1
    }

    function selectLongShortcut(path) {
        configurationPowermenu.longShortcut = path
    }

    function selectShortShortcut(path) {
        configurationPowermenu.shortShortcut = path
    }

    function selectDoubleShortcut(path) {
        configurationPowermenu.doubleShortcut = path
    }

    function selectLongShortcutLocked(path) {
        configurationPowermenu.longShortcutLocked = path
    }

    function selectShortShortcutLocked(path) {
        configurationPowermenu.shortShortcutLocked = path
    }

    function selectDoubleShortcutLocked(path) {
        configurationPowermenu.doubleShortcutLocked = path
    }

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: column.height

        PullDownMenu {
            background: Component { ShaderTiledBackground {} }
            id: pullDown
            MenuItem {
                text: qsTr("Restart lipstick")
                onClicked: {
                    helper.doRestartLipstick()
                }
            }
            MenuLabel {
                text: qsTr("Restart lipstick after first install")
            }
        }

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Configuration")
            }

            ComboBox {
                id: longAction
                width: parent.width
                label: qsTr("Longtap action")
                currentIndex: getActionIndex(helper.longPressAction)

                menu: ContextMenu {
                    MenuItem { text: qsTr("Disabled") }
                    MenuItem { text: qsTr("Poweroff") }
                    MenuItem { text: qsTr("Soft poweroff") }
                    MenuItem { text: qsTr("Lock") }
                    MenuItem { text: qsTr("Unlock") }
                    MenuItem { text: qsTr("Lock/Unlock") }
                    MenuItem { text: qsTr("Menu") }
                    MenuItem { text: qsTr("Application") }
                }

                onCurrentItemChanged: {
                    if (pageStack.currentPage.objectName === "configurationPage" || pageStack.currentPage.objectName === "") {
                        switch (currentIndex) {
                        case 1:
                            helper.longPressAction = "poweroff"
                            return
                        case 2:
                            helper.longPressAction = "softpoweroff"
                            return
                        case 3:
                            helper.longPressAction = "tklock-lock"
                            return
                        case 4:
                            helper.longPressAction = "tklock-unlock"
                            return
                        case 5:
                            helper.longPressAction = "tklock-both"
                            return
                        case 6:
                            helper.longPressAction = "dbus-signal-powerkeyMenu"
                            return
                        case 7:
                            helper.longPressAction = "dbus-signal-actionLong"
                            return
                        default:
                            helper.longPressAction = "disabled"
                            return
                        }
                    }
                }
            }

            ValueButton {
                visible: longAction.currentIndex == 7
                label: qsTr("Phone unlocked application")
                value: helper.readDesktopName(configurationPowermenu.longShortcut)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.longShortcut], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectLongShortcut)
                }
            }

            ValueButton {
                visible: longAction.currentIndex == 7
                label: qsTr("Phone locked application")
                value: helper.readDesktopName(configurationPowermenu.longShortcutLocked)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.longShortcutLocked], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectLongShortcutLocked)
                }
            }

            ComboBox {
                id: shortAction
                width: parent.width
                label: qsTr("Shorttap action")
                currentIndex: getActionIndex(helper.shortPressAction)

                menu: ContextMenu {
                    MenuItem { text: qsTr("Disabled") }
                    MenuItem { text: qsTr("Poweroff") }
                    MenuItem { text: qsTr("Soft poweroff") }
                    MenuItem { text: qsTr("Lock") }
                    MenuItem { text: qsTr("Unlock") }
                    MenuItem { text: qsTr("Lock/Unlock") }
                    MenuItem { text: qsTr("Menu") }
                    MenuItem { text: qsTr("Application") }
                }

                onCurrentItemChanged: {
                    if (pageStack.currentPage.objectName === "configurationPage" || pageStack.currentPage.objectName === "") {
                        switch (currentIndex) {
                        case 1:
                            helper.shortPressAction = "poweroff"
                            return
                        case 2:
                            helper.shortPressAction = "softpoweroff"
                            return
                        case 3:
                            helper.shortPressAction = "tklock-lock"
                            return
                        case 4:
                            helper.shortPressAction = "tklock-unlock"
                            return
                        case 5:
                            helper.shortPressAction = "tklock-both"
                            return
                        case 6:
                            helper.shortPressAction = "dbus-signal-powerkeyMenu"
                            return
                        case 7:
                            helper.shortPressAction = "dbus-signal-actionShort"
                            return
                        default:
                            helper.shortPressAction = "disabled"
                            return
                        }
                    }
                }
            }

            ValueButton {
                visible: shortAction.currentIndex == 7
                label: qsTr("Phone unlocked application")
                value: helper.readDesktopName(configurationPowermenu.shortShortcut)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.shortShortcut], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortShortcut)
                }
            }

            ValueButton {
                visible: shortAction.currentIndex == 7
                label: qsTr("Phone locked application")
                value: helper.readDesktopName(configurationPowermenu.shortShortcutLocked)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.shortShortcutLocked], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectShortShortcutLocked)
                }
            }

            ComboBox {
                id: doubleAction
                width: parent.width
                label: qsTr("Doubletap action")
                currentIndex: getActionIndex(helper.doublePressAction)

                menu: ContextMenu {
                    MenuItem { text: qsTr("Disabled") }
                    MenuItem { text: qsTr("Poweroff") }
                    MenuItem { text: qsTr("Soft poweroff") }
                    MenuItem { text: qsTr("Lock") }
                    MenuItem { text: qsTr("Unlock") }
                    MenuItem { text: qsTr("Lock/Unlock") }
                    MenuItem { text: qsTr("Menu") }
                    MenuItem { text: qsTr("Application") }
                }

                onCurrentItemChanged: {
                    if (pageStack.currentPage.objectName === "configurationPage" || pageStack.currentPage.objectName === "") {
                        switch (currentIndex) {
                        case 1:
                            helper.doublePressAction = "poweroff"
                            return
                        case 2:
                            helper.doublePressAction = "softpoweroff"
                            return
                        case 3:
                            helper.doublePressAction = "tklock-lock"
                            return
                        case 4:
                            helper.doublePressAction = "tklock-unlock"
                            return
                        case 5:
                            helper.doublePressAction = "tklock-both"
                            return
                        case 6:
                            helper.doublePressAction = "dbus-signal-powerkeyMenu"
                            return
                        case 7:
                            helper.doublePressAction = "dbus-signal-actionDouble"
                            return
                        default:
                            helper.doublePressAction = "disabled"
                            return
                        }
                    }
                }
            }

            ValueButton {
                visible: doubleAction.currentIndex == 7
                label: qsTr("Phone unlocked application")
                value: helper.readDesktopName(configurationPowermenu.doubleShortcut)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.doubleShortcut], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectDoubleShortcut)
                }
            }

            ValueButton {
                visible: doubleAction.currentIndex == 7
                label: qsTr("Phone locked application")
                value: helper.readDesktopName(configurationPowermenu.doubleShortcutLocked)
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: [configurationPowermenu.doubleShortcutLocked], showHidden: configurationPowermenu.showHiddenShortcuts})
                    selector.selected.connect(page.selectDoubleShortcutLocked)
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Shutdown controls")
                currentIndex: configurationPowermenu.showShutdown ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Shown")
                        onClicked: {
                            configurationPowermenu.showShutdown = true
                        }
                    }
                    MenuItem {
                        text: qsTr("Hidden")
                        onClicked: {
                            configurationPowermenu.showShutdown = false
                        }
                    }
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Show hidden shortcuts")
                description: qsTr("For application selector only")
                currentIndex: configurationPowermenu.showHiddenShortcuts ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Yes")
                        onClicked: {
                            configurationPowermenu.showHiddenShortcuts = true
                        }
                    }
                    MenuItem {
                        text: qsTr("No")
                        onClicked: {
                            configurationPowermenu.showHiddenShortcuts = false
                        }
                    }
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Fancy menu background")
                description: qsTr("Requres lipstick restart")
                currentIndex: configurationPowermenu.fancyBackground ? 0 : 1

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Yes")
                        onClicked: {
                            configurationPowermenu.fancyBackground = true
                        }
                    }
                    MenuItem {
                        text: qsTr("No")
                        onClicked: {
                            configurationPowermenu.fancyBackground = false
                        }
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }

    ConfigurationGroup {
        id: configurationPowermenu
        path: "/apps/powermenu"
        property bool showShutdown: true
        property string longShortcut: ""
        property string shortShortcut: ""
        property string doubleShortcut: ""
        property string longShortcutLocked: ""
        property string shortShortcutLocked: ""
        property string doubleShortcutLocked: ""
        property bool showHiddenShortcuts: false
        property bool fancyBackground: true
    }
}
