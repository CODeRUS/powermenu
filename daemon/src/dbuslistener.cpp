#include "dbuslistener.h"

#include <QDesktopServices>
#include <QDebug>
#include <mlite5/MGConfItem>

DBusListener::DBusListener(QObject *parent) :
    QObject(parent),
    _shouldRestartLipstick(false)
{
    QFile compositorQml("/usr/share/lipstick-jolla-home-qt5/compositor.qml");
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

    QDBusConnection::systemBus().connect("", MCE_SIGNAL_PATH, MCE_SIGNAL_IFACE,
                                         "power_button_trigger", this, SLOT(powerButtonTrigger(QString)));

    iface = new QDBusInterface("com.jolla.lipstick.PowerMenuDialog",
                               "/org/coderus/powermenu",
                               "com.jolla.lipstick.PowerMenuDialogIf",
                               QDBusConnection::sessionBus(), this);

    mce = new QDBusInterface(MCE_SERVICE,
                             MCE_REQUEST_PATH,
                             MCE_REQUEST_IFACE,
                             QDBusConnection::systemBus(), this);


    qDebug() << "DBus service" << (QDBusConnection::sessionBus().registerService("org.coderus.powermenu") ? "registered" : "error!");
    qDebug() << "DBus object" << (QDBusConnection::sessionBus().registerObject("/", this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties)
                ? "registered" : "error!");

    qDebug() << "listener started";

    setAction1("application1");
    setAction2("powermenu");
}

QString DBusListener::getAction1()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION1).toString();
}

void DBusListener::setAction1(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION1, action);
}

QString DBusListener::getAction2()
{
    return getMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION2).toString();
}

void DBusListener::setAction2(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_DBUS_ACTION2, action);
}

int DBusListener::getLongPressDelay()
{
    return getMceValue(MCE_GCONF_POWERKEY_LONG_PRESS_DELAY).toInt();
}

void DBusListener::setLongPressDelay(int msecs)
{
    setMceValue(MCE_GCONF_POWERKEY_LONG_PRESS_DELAY, QString::number(msecs));
}

QString DBusListener::getLongPressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_ON).toString();
}

QString DBusListener::getLongPressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_OFF).toString();
}

void DBusListener::setLongPressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_ON, action);
}

void DBusListener::setLongPressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_LONG_OFF, action);
}

int DBusListener::getDoublePressDelay()
{
    return getMceValue(MCE_GCONF_POWERKEY_DOUBLE_PRESS_DELAY).toInt();
}

void DBusListener::setDoublePressDelay(int msecs)
{
    setMceValue(MCE_GCONF_POWERKEY_DOUBLE_PRESS_DELAY, QString::number(msecs));
}

QString DBusListener::getDoublePressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_ON).toString();
}

QString DBusListener::getDoublePressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_OFF).toString();
}

void DBusListener::setDoublePressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_ON, action);
}

void DBusListener::setDoublePressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_DOUBLE_OFF, action);
}

QString DBusListener::getShortPressActionOn()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_ON).toString();
}

QString DBusListener::getShortPressActionOff()
{
    return getMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_OFF).toString();
}

void DBusListener::setShortPressActionOn(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_ON, action);
}

void DBusListener::setShortPressActionOff(const QString &action)
{
    setMceValue(MCE_GCONF_POWERKEY_ACTIONS_SINGLE_OFF, action);
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
    QDBusReply<QVariant> reply = mce->call(QDBus::AutoDetect, "get_config", key);
    if (reply.isValid()) {
        return reply.value();
    }
    else {
        return QVariant();
    }
}

void DBusListener::setMceValue(const QString &key, const QVariant &value)
{
    mce->call(QDBus::AutoDetect, "set_config", key, QVariant::fromValue(QDBusVariant(value)));
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

void DBusListener::powerButtonTrigger(const QString &triggerName)
{
    qDebug() << "powerButtonTrigger" << triggerName;
    if (triggerName == "powermenu") {
        openPowerMenu();
    }
    else if (triggerName == "application1") {
        MGConfItem shortcut1("/apps/powermenu/applicationShortcut1");
        if (!shortcut1.value().isNull()) {
            openDesktop(shortcut1.value().toString());
        }
    }
    else if (triggerName == "application2") {
        MGConfItem shortcut2("/apps/powermenu/applicationShortcut2");
        if (!shortcut2.value().isNull()) {
            openDesktop(shortcut2.value().toString());
        }
    }
}
