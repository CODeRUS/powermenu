#ifndef DBUSLISTENER_H
#define DBUSLISTENER_H

#include <QObject>

#include <QtDBus>

#define POWERMENU_MCE "/etc/mce/90-powermenu-keys.ini"

#define POWERKEY_LONGDELAY "PowerKey/PowerKeyLongDelay"
#define POWERKEY_LONGACTION "PowerKey/PowerKeyLongAction"
#define POWERKEY_DOUBLEDELAY "PowerKey/PowerKeyDoubleDelay"
#define POWERKEY_DOUBLEACTION "PowerKey/PowerKeyDoubleAction"
#define POWERKEY_MEDIUMDELAY "PowerKey/PowerKeyMediumDelay"
#define POWERKEY_SHORTACTION "PowerKey/PowerKeyShortAction"

#define MCE_DBUS_PATH "/com/nokia/mce/request"
#define MCE_DBUS_IFACE "com.nokia.mce.request"

class DBusListener : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.powermenu")
public:
    explicit DBusListener(QObject *parent = 0);

    Q_PROPERTY(int longPressDelay READ getLongPressDelay WRITE setLongPressDelay FINAL)
    Q_SCRIPTABLE int getLongPressDelay();
    Q_SCRIPTABLE void setLongPressDelay(int msecs);

    Q_PROPERTY(QString longPressAction READ getLongPressAction WRITE setLongPressAction FINAL)
    Q_SCRIPTABLE QString getLongPressAction();
    Q_SCRIPTABLE void setLongPressAction(const QString &action);

    Q_PROPERTY(int doublePressDelay READ getDoublePressDelay WRITE setDoublePressDelay FINAL)
    Q_SCRIPTABLE int getDoublePressDelay();
    Q_SCRIPTABLE void setDoublePressDelay(int msecs);

    Q_PROPERTY(QString doublePressAction READ getDoublePressAction WRITE setDoublePressAction FINAL)
    Q_SCRIPTABLE QString getDoublePressAction();
    Q_SCRIPTABLE void setDoublePressAction(const QString &action);

    Q_PROPERTY(int mediumPressDelay READ getMediumPressDelay WRITE setMediumPressDelay FINAL)
    Q_SCRIPTABLE int getMediumPressDelay();
    Q_SCRIPTABLE void setMediumPressDelay(int msecs);

    Q_PROPERTY(QString shortPressAction READ getShortPressAction WRITE setShortPressAction FINAL)
    Q_SCRIPTABLE QString getShortPressAction();
    Q_SCRIPTABLE void setShortPressAction(const QString &action);

    Q_PROPERTY(bool shouldRestartLipstick READ shouldRestartLipstick FINAL)
    Q_SCRIPTABLE bool shouldRestartLipstick();

    Q_SCRIPTABLE void openDesktop(const QString &desktop);
    Q_SCRIPTABLE void doRestartLipstick();

    Q_SCRIPTABLE void ready();

signals:
    void restartLipstick();

private:
    QVariant getMceValue(const QString &key);
    void setMceValue(const QString &key, const QVariant &value);

    void restartSystemService(const QString &serviceName);
    void restartUserService(const QString &serviceName);

    void openPowerMenu();

    QDBusInterface *iface;

    bool _shouldRestartLipstick;

private slots:
    void powerkeyMenuRequested();
    void powerkeyLongPressed();
    void powerkeyShortPressed();
    void powerkeyDoublePressed();
};

#endif // DBUSLISTENER_H
