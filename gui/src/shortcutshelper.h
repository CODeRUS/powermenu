#ifndef SHORTCUTSHELPER_H
#define SHORTCUTSHELPER_H

#include <QObject>

#include <QtDBus>

#define AUTOSTART_DIR "%1/.config/systemd/user/post-user-session.target.wants"
#define AUTOSTART_USER "%1/.config/systemd/user/post-user-session.target.wants/powermenu.service"
#define AUTOSTART_SERVICE "/usr/lib/systemd/user/powermenu.service"

class ShortcutsHelper : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutsHelper(QObject *parent = 0);

    Q_PROPERTY(int longPressDelay READ getLongPressDelay WRITE setLongPressDelay NOTIFY longPressDelayChanged)
    int getLongPressDelay();
    void setLongPressDelay(int msecs);

    Q_PROPERTY(QString longPressAction READ getLongPressAction WRITE setLongPressAction NOTIFY longPressActionChanged)
    QString getLongPressAction();
    void setLongPressAction(const QString &action);

    Q_PROPERTY(int doublePressDelay READ getDoublePressDelay WRITE setDoublePressDelay NOTIFY doublePressDelayChanged)
    int getDoublePressDelay();
    void setDoublePressDelay(int msecs);

    Q_PROPERTY(QString doublePressAction READ getDoublePressAction WRITE setDoublePressAction NOTIFY doublePressActionChanged)
    QString getDoublePressAction();
    void setDoublePressAction(const QString &action);

    Q_PROPERTY(int mediumPressDelay READ getMediumPressDelay WRITE setMediumPressDelay NOTIFY mediumPressDelayChanged)
    int getMediumPressDelay();
    void setMediumPressDelay(int msecs);

    Q_PROPERTY(QString shortPressAction READ getShortPressAction WRITE setShortPressAction NOTIFY shortPressActionChanged)
    QString getShortPressAction();
    void setShortPressAction(const QString &action);

    Q_PROPERTY(bool shouldRestartLipstick READ shouldRestartLipstick NOTIFY shouldRestartLipstickChanged)
    bool shouldRestartLipstick();

    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    QString version() const;

    Q_INVOKABLE bool checkAutostart();
    Q_INVOKABLE void setAutostart(bool enabled);
    Q_INVOKABLE void doRestartLipstick();

    Q_INVOKABLE QString readDesktopName(const QString &path) const;

private:
    QDBusInterface *iface;
    QString _version;

private slots:
    void checkVersion();

signals:
    void versionChanged();

    void longPressDelayChanged();
    void longPressActionChanged();
    void doublePressDelayChanged();
    void doublePressActionChanged();
    void mediumPressDelayChanged();
    void shortPressActionChanged();
    void shouldRestartLipstickChanged();
};

#endif // SHORTCUTSHELPER_H
