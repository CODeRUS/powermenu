TARGET = powermenu-gui
target.path = /usr/bin

QT += dbus

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

desktops.files = powermenu.desktop
desktops.path = /usr/share/applications

icons.files = powermenu.png
icons.path = /usr/share/icons/hicolor/86x86/apps

qmls.files = qmls
qmls.path = /usr/share/powermenu-gui

images.files = images
images.path = /usr/share/powermenu-gui

INSTALLS = target desktops icons qmls images

SOURCES += \
    src/main.cpp \
    src/shortcutshelper.cpp

HEADERS += \
    src/shortcutshelper.h

OTHER_FILES += \
    qmls/ShaderTiledBackground.qml
