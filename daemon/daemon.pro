TARGET = powermenu-daemon
target.path = /usr/bin

QT += dbus

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

dbus.files = dbus/org.coderus.powermenu.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/powermenu.service
systemd.path = /usr/lib/systemd/user

lipstick.files = lipstick/PowerMenuDialog.qml
lipstick.path = /usr/share/lipstick-jolla-home-qt5

patch.files = lipstick/lipstick.patch
patch.path = /usr/share/powermenu-gui/data

INSTALLS = target systemd lipstick patch dbus

SOURCES += \
    src/dbuslistener.cpp \
    src/main.cpp

HEADERS += \
    src/dbuslistener.h
