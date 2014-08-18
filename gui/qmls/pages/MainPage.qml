import QtQuick 2.1
import Sailfish.Silica 1.0
import org.coderus.desktopfilemodel 1.0
import org.nemomobile.configuration 1.0

Page {
    id: page
    objectName: "mainPage"

    function addShortcut(path) {
        var list = []
        list = shortcutsConfig.value
        list.splice(list.length, 0, path)
        shortcutsConfig.value = list
    }

    function moveShortcutUp(index) {
        var list = []
        list = shortcutsConfig.value
        var temp = list[index - 1]
        list[index - 1] = list[index]
        list[index] = temp
        shortcutsConfig.value = list
    }

    function moveShortcutDown(index) {
        var list = []
        list = shortcutsConfig.value
        var temp = list[index + 1]
        list[index + 1] = list[index]
        list[index] = temp
        shortcutsConfig.value = list
    }

    SilicaListView {
        id: view
        anchors.fill: page
        delegate: shortcutDelegate
        model: desktopModel
        header: PageHeader {
            id: title
            title: qsTr("Selected shortcuts")
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                }
            }

            MenuItem {
                text: qsTr("Configuration")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ConfigurationPage.qml"))
                }
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Add shortcut")
                onClicked: {
                    var selector = pageStack.push(Qt.resolvedUrl("ShortcutsPage.qml"), {selectedValues: shortcutsConfig.value})
                    selector.selected.connect(page.addShortcut)
                }
            }
        }

        ViewPlaceholder {
            id: placeholder
            enabled: view.count == 0
            text: qsTr("No shortcuts selected")

            Component {
                id: activeContent
                PulleyAnimationHint {
                    flickable: placeholder.flickable
                    width: parent.width
                    height: width
                    anchors.centerIn: parent
                    pushUpHint: true
                }
            }

            Component.onCompleted: {
                placeholder._content = activeContent.createObject(placeholder)
            }
        }

        VerticalScrollDecorator {}
    }

    ConfigurationValue {
        id: shortcutsConfig
        key: "/apps/powermenu/shortcuts"
        defaultValue: []
    }

    DesktopFileSortModel {
        id: desktopModel
        filterShortcuts: shortcutsConfig.value
        onlySelected: true
    }

    Component {
        id: shortcutDelegate
        ListItem {
            id: item
            width: ListView.view.width
            contentHeight: 110
            ListView.onRemove: animateRemoval(item)
            menu: contextMenu

            function removeShortcut() {
                remorseAction(qsTr("Delete shortcut"),
                                     function() {
                                         var list = []
                                         list = shortcutsConfig.value
                                         list.splice(model.index, 1)
                                         shortcutsConfig.value = list
                                     }
                )
            }

            Image {
                id: iconImage
                source: model.icon
                width: 86
                height: 86
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
            }

            IconButton {
                id: moveDown
                icon.source: "image://theme/icon-m-down"
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                    verticalCenter: iconImage.verticalCenter
                }
                visible: model.index < desktopModel.count - 1
                onClicked: {
                    moveShortcutDown(model.index)
                }
            }

            IconButton {
                id: moveUp
                icon.source: "image://theme/icon-m-up"
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingMedium + Theme.itemSizeSmall
                    verticalCenter: iconImage.verticalCenter
                }
                visible: model.index > 0
                onClicked: {
                    moveShortcutUp(model.index)
                }
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: {
                            removeShortcut()
                        }
                    }
                }
            }
        }
    }
}


