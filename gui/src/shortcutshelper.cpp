#include "shortcutshelper.h"

#include <QTimer>
#include <QDebug>
#include <mlite5/MDesktopEntry>

ShortcutsHelper::ShortcutsHelper(QObject *parent) :
    QObject(parent)
{
    QTimer::singleShot(1, this, SLOT(checkVersion()));

    iface = new QDBusInterface("org.coderus.powermenu", "/", "org.coderus.powermenu", QDBusConnection::sessionBus(), this);
}

int ShortcutsHelper::getLongPressDelay()
{
    if (iface) {
        QDBusReply<int> reply = iface->call("getLongPressDelay");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return 1500;
}

void ShortcutsHelper::setLongPressDelay(int msecs)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressDelay", msecs);
    }
}

QString ShortcutsHelper::getLongPressAction()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getLongPressAction");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "poweroff";
}

void ShortcutsHelper::setLongPressAction(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressAction", action);
    }
}

int ShortcutsHelper::getDoublePressDelay()
{
    if (iface) {
        QDBusReply<int> reply = iface->call("getDoublePressDelay");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return 500;
}

void ShortcutsHelper::setDoublePressDelay(int msecs)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressDelay", msecs);
    }
}

QString ShortcutsHelper::getDoublePressAction()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getDoublePressAction");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "disabled";
}

void ShortcutsHelper::setDoublePressAction(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressAction", action);
    }
}

int ShortcutsHelper::getMediumPressDelay()
{
    if (iface) {
        QDBusReply<int> reply = iface->call("getMediumPressDelay");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return 1000;
}

void ShortcutsHelper::setMediumPressDelay(int msecs)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setMediumPressDelay", msecs);
    }
}

QString ShortcutsHelper::getShortPressAction()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getShortPressAction");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "tklock-lock";
}

void ShortcutsHelper::setShortPressAction(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setShortPressAction", action);
    }
}

bool ShortcutsHelper::shouldRestartLipstick()
{
    if (iface) {
        QDBusReply<bool> reply = iface->call("shouldRestartLipstick");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return false;
}

QString ShortcutsHelper::version() const
{
    return _version;
}

bool ShortcutsHelper::checkAutostart()
{
    QString autostartUser = QString(AUTOSTART_USER).arg(QDir::homePath());
    QFile service(autostartUser);
    return service.exists();
}

void ShortcutsHelper::setAutostart(bool enabled)
{
    QString autostartUser = QString(AUTOSTART_USER).arg(QDir::homePath());
    if (enabled) {
        QString autostartDir = QString(AUTOSTART_DIR).arg(QDir::homePath());
        QDir dir(autostartDir);
        if (!dir.exists())
            dir.mkpath(autostartDir);
        QFile service(AUTOSTART_SERVICE);
        service.link(autostartUser);
    }
    else {
        QFile service(autostartUser);
        if (service.exists()) {
            service.remove();
        }
    }
}

void ShortcutsHelper::doRestartLipstick()
{
    if (iface) {
        iface->call(QDBus::NoBlock, "doRestartLipstick");
    }
}

QString ShortcutsHelper::readDesktopName(const QString &path) const
{
    MDesktopEntry desktop(path);
    if (desktop.isValid()) {
        return desktop.name();
    }
    return path;
}

void ShortcutsHelper::checkVersion()
{
    QProcess app;
    app.start("/bin/rpm", QStringList() << "-qa" << "--queryformat" << "%{version}" <<  "powermenu");
    if (app.bytesAvailable() <= 0) {
        app.waitForFinished(5000);
    }
    _version = QString::fromLocal8Bit(app.readAll().constData());
    Q_EMIT versionChanged();
}
