#include "dbuslistener.h"

#include <QDesktopServices>
#include <QDebug>
#include <mlite5/MGConfItem>

DBusListener::DBusListener(QObject *parent) :
    QObject(parent),
    _shouldRestartLipstick(false),
    _phoneLocked(false)
{
    QFile compositorQml("/usr/share/lipstick-jolla-home-qt5/qml/compositor.qml");
    if (compositorQml.exists() && compositorQml.open(QFile::ReadWrite)) {
        QByteArray data = compositorQml.readAll();
        if (!data.contains("PowerMenuDialog") && data.contains("UnresponsiveApplicationDialog")) {
            data.replace("UnresponsiveApplicationDialog {", "PowerMenuDialog {}\n\n    UnresponsiveApplicationDialog {");
            compositorQml.resize(0);
            compositorQml.seek(0);
            compositorQml.write(data);
            compositorQml.close();
            //restartUserService("lipstick.service");
            _shouldRestartLipstick = true;
            qDebug() << "compositor patched, please restart lipstick!";
        }
        else {
            qWarning() << "can't patch!";
        }
    }

    QDBusReply<QString> reply = QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IFACE,
                                               QDBusConnection::systemBus()).call("get_tklock_mode");
    if (reply.isValid()) {
        _phoneLocked = (reply.value() == "locked");
    }

    QDBusConnection::systemBus().connect("", MCE_REQUEST_PATH, MCE_REQUEST_IFACE,
                                         "powerkeyMenu", this, SLOT(powerkeyMenuRequested()));
    QDBusConnection::systemBus().connect("", MCE_REQUEST_PATH, MCE_REQUEST_IFACE,
                                         "actionLong", this, SLOT(powerkeyLongPressed()));
    QDBusConnection::systemBus().connect("", MCE_REQUEST_PATH, MCE_REQUEST_IFACE,
                                         "actionShort", this, SLOT(powerkeyShortPressed()));
    QDBusConnection::systemBus().connect("", MCE_REQUEST_PATH, MCE_REQUEST_IFACE,
                                         "actionDouble", this, SLOT(powerkeyDoublePressed()));
    QDBusConnection::systemBus().connect("", MCE_SIGNAL_PATH, MCE_SIGNAL_IFACE,
                                         "tklock_mode_ind", this, SLOT(tklockChanged(QString)));

    iface = new QDBusInterface("com.jolla.lipstick.PowerMenuDialog",
                               "/org/coderus/powermenu",
                               "com.jolla.lipstick.PowerMenuDialogIf",
                               QDBusConnection::sessionBus(), this);


    qDebug() << "DBus service" << (QDBusConnection::sessionBus().registerService("org.coderus.powermenu") ? "registered" : "error!");
    qDebug() << "DBus object" << (QDBusConnection::sessionBus().registerObject("/", this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties)
                ? "registered" : "error!");

    qDebug() << "listener started";
}

int DBusListener::getLongPressDelay()
{
    return getMceValue(POWERKEY_LONGDELAY).toInt();
}

void DBusListener::setLongPressDelay(int msecs)
{
    setMceValue(POWERKEY_LONGDELAY, msecs);
}

QString DBusListener::getLongPressAction()
{
    return getMceValue(POWERKEY_LONGACTION).toString();
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


void DBusListener::setLongPressAction(const QString &action)
{
    setMceValue(POWERKEY_LONGACTION, action);
}

int DBusListener::getDoublePressDelay()
{
    return getMceValue(POWERKEY_DOUBLEDELAY).toInt();
}

void DBusListener::setDoublePressDelay(int msecs)
{
    setMceValue(POWERKEY_DOUBLEDELAY, msecs);
}

QString DBusListener::getDoublePressAction()
{
    return getMceValue(POWERKEY_DOUBLEACTION).toString();
}

void DBusListener::setDoublePressAction(const QString &action)
{
    setMceValue(POWERKEY_DOUBLEACTION, action);
}

int DBusListener::getMediumPressDelay()
{
    return getMceValue(POWERKEY_MEDIUMDELAY).toInt();
}

void DBusListener::setMediumPressDelay(int msecs)
{
    setMceValue(POWERKEY_MEDIUMDELAY, msecs);
}

QString DBusListener::getShortPressAction()
{
    return getMceValue(POWERKEY_SHORTACTION).toString();
}

void DBusListener::setShortPressAction(const QString &action)
{
    setMceValue(POWERKEY_SHORTACTION, action);
}

bool DBusListener::shouldRestartLipstick()
{
    return _shouldRestartLipstick;
}

void DBusListener::openDesktop(const QString &desktop)
{
    if (desktop.endsWith(".desktop")) {
        if (QFile(desktop).exists()) {
            qDebug() << "starting desktop:" << desktop;
            QDesktopServices::openUrl(QUrl::fromLocalFile(desktop));
        }
        else {
            qWarning() << "file not exists:" << desktop;
        }
    }
    else {
        qWarning() << "not a desktop file:" << desktop;
    }
}

void DBusListener::doRestartLipstick()
{
    restartUserService("lipstick.service");
}

void DBusListener::ready()
{
    qDebug() << "Hello!";
}

QVariant DBusListener::getMceValue(const QString &key)
{
    QSettings mceini(POWERMENU_MCE, QSettings::IniFormat);
    qDebug() << "getMceValue" << key << mceini.value(key);
    return mceini.value(key);
}

void DBusListener::setMceValue(const QString &key, const QVariant &value)
{
    qDebug() << "setMceValue" << key << value;

    QSettings mceini(POWERMENU_MCE, QSettings::IniFormat);
    mceini.setValue(key, value);
    mceini.sync();

    restartSystemService("mce.service");
}

void DBusListener::restartSystemService(const QString &serviceName)
{
    QDBusInterface systemd("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager", QDBusConnection::systemBus());
    qDebug() << "systemd reply:" << systemd.call(QDBus::NoBlock, "RestartUnit", serviceName, "replace").errorMessage();
}

void DBusListener::restartUserService(const QString &serviceName)
{
    QDBusInterface systemd("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager", QDBusConnection::sessionBus());
    qDebug() << "systemd reply:" << systemd.call(QDBus::NoBlock, "RestartUnit", serviceName, "replace").errorMessage();
}

void DBusListener::openPowerMenu()
{
    if (iface) {
        iface->call(QDBus::NoBlock, "openDialog");
    }
}

void DBusListener::powerkeyMenuRequested()
{
    openPowerMenu();
}

void DBusListener::powerkeyLongPressed()
{
    if (_phoneLocked) {
        MGConfItem longShortcut("/apps/powermenu/longShortcutLocked");
        if (!longShortcut.value().isNull()) {
            openDesktop(longShortcut.value().toString());
        }
    }
    else {
        MGConfItem longShortcut("/apps/powermenu/longShortcut");
        if (!longShortcut.value().isNull()) {
            openDesktop(longShortcut.value().toString());
        }
    }
}

void DBusListener::powerkeyShortPressed()
{
    if (_phoneLocked) {
        MGConfItem shortShortcut("/apps/powermenu/shortShortcutLocked");
        if (!shortShortcut.value().isNull()) {
            openDesktop(shortShortcut.value().toString());
        }
    }
    else {
        MGConfItem shortShortcut("/apps/powermenu/shortShortcut");
        if (!shortShortcut.value().isNull()) {
            openDesktop(shortShortcut.value().toString());
        }
    }
}

void DBusListener::powerkeyDoublePressed()
{
    if (_phoneLocked) {
        MGConfItem doubleShortcut("/apps/powermenu/doubleShortcutLocked");
        if (!doubleShortcut.value().isNull()) {
            openDesktop(doubleShortcut.value().toString());
        }
    }
    else {
        MGConfItem doubleShortcut("/apps/powermenu/doubleShortcut");
        if (!doubleShortcut.value().isNull()) {
            openDesktop(doubleShortcut.value().toString());
        }
    }
}

void DBusListener::tklockChanged(const QString &mode)
{
    _phoneLocked = (mode == "locked");
}
