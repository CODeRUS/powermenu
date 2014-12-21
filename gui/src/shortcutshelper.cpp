#include "shortcutshelper.h"

#include <QTimer>
#include <QDebug>
#include <mlite5/MDesktopEntry>
#include <QSettings>

ShortcutsHelper::ShortcutsHelper(QObject *parent) :
    QObject(parent)
{
    QTimer::singleShot(1, this, SLOT(checkVersion()));
    QTimer::singleShot(1, this, SLOT(createInterface()));

    nam = new QNetworkAccessManager(this);

    QSettings settings;
    settings.sync();
    QString code = settings.value("code", "demo").toString();
    checkActivation(code);
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
        Q_EMIT longPressDelayChanged();
    }
}

QString ShortcutsHelper::getLongPressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getLongPressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "shutdown";
}

QString ShortcutsHelper::getLongPressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getLongPressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "";
}

void ShortcutsHelper::setLongPressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressActionOn", action);
        Q_EMIT longPressActionOnChanged();
    }
}

void ShortcutsHelper::setLongPressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setLongPressActionOff", action);
        Q_EMIT longPressActionOffChanged();
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
        Q_EMIT doublePressDelayChanged();
    }
}

QString ShortcutsHelper::getDoublePressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getDoublePressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "blank,tklock,devlock";
}

QString ShortcutsHelper::getDoublePressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getDoublePressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "unblank,tkunlock";
}

void ShortcutsHelper::setDoublePressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressActionOn", action);
        Q_EMIT doublePressActionOnChanged();
    }
}

void ShortcutsHelper::setDoublePressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setDoublePressActionOff", action);
        Q_EMIT doublePressActionOffChanged();
    }
}

QString ShortcutsHelper::getShortPressActionOn()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getShortPressActionOn");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "blank,tklock";
}

QString ShortcutsHelper::getShortPressActionOff()
{
    if (iface) {
        QDBusReply<QString> reply = iface->call("getShortPressActionOff");
        if (reply.isValid()) {
            return reply.value();
        }
    }
    return "unblank";
}

void ShortcutsHelper::setShortPressActionOn(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setShortPressActionOn", action);
        Q_EMIT shortPressActionOnChanged();
    }
}

void ShortcutsHelper::setShortPressActionOff(const QString &action)
{
    if (iface) {
        iface->call(QDBus::NoBlock, "setShortPressActionOff", action);
        Q_EMIT shortPressActionOffChanged();
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

QString ShortcutsHelper::bannerPath() const
{
    return _bannerPath;
}

void ShortcutsHelper::checkActivation(const QString &code)
{
    QSettings settings;
    settings.setValue("code", code);
    settings.sync();

    QString url(QByteArray::fromBase64("aHR0cHM6Ly9jb2RlcnVzLm9wZW5yZXBvcy5uZXQvd2hpdGVzb2Z0L2xncmVtb3RlLyUx="));
    QObject::connect(nam->get(QNetworkRequest(QUrl(url.arg(code)))), SIGNAL(finished()), this, SLOT(onActivationReply()));
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

void ShortcutsHelper::onActivationReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        _bannerPath = QString::fromUtf8(reply->readAll());
        Q_EMIT bannerPathChanged();
    }
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

void ShortcutsHelper::createInterface()
{
    iface = new QDBusInterface("org.coderus.powermenu", "/", "org.coderus.powermenu", QDBusConnection::sessionBus(), this);
}
