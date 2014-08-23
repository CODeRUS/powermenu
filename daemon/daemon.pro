TARGET = powermenu-daemon
target.path = /usr/bin

QT += dbus

CONFIG += sailfishapp link_pkgconfig
PKGCONFIG += sailfishapp mlite5

dbus.files = dbus/org.coderus.powermenu.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/powermenu.service
systemd.path = /usr/lib/systemd/user

mce.files = mce/90-powermenu-keys.ini
mce.path = /etc/mce

lipstick.files = lipstick/PowerMenuDialog.qml
lipstick.path = /usr/share/lipstick-jolla-home-qt5/qml

patch.files = lipstick/lipstick.patch \
              lipstick/lipstick_up.patch
patch.path = /usr/share/powermenu-gui/data

INSTALLS = target systemd mce lipstick patch dbus

SOURCES += \
    src/dbuslistener.cpp \
    src/main.cpp

HEADERS += \
    src/dbuslistener.h
