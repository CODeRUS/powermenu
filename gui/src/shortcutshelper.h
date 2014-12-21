#ifndef SHORTCUTSHELPER_H
#define SHORTCUTSHELPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
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

    Q_PROPERTY(QString longPressActionOn READ getLongPressActionOn WRITE setLongPressActionOn NOTIFY longPressActionOnChanged)
    QString getLongPressActionOn();
    void setLongPressActionOn(const QString &action);

    Q_PROPERTY(QString longPressActionOff READ getLongPressActionOff WRITE setLongPressActionOff NOTIFY longPressActionOffChanged)
    QString getLongPressActionOff();
    void setLongPressActionOff(const QString &action);

    Q_PROPERTY(int doublePressDelay READ getDoublePressDelay WRITE setDoublePressDelay NOTIFY doublePressDelayChanged)
    int getDoublePressDelay();
    void setDoublePressDelay(int msecs);

    Q_PROPERTY(QString doublePressActionOn READ getDoublePressActionOn WRITE setDoublePressActionOn NOTIFY doublePressActionOnChanged)
    QString getDoublePressActionOn();
    void setDoublePressActionOn(const QString &action);

    Q_PROPERTY(QString doublePressActionOff READ getDoublePressActionOff WRITE setDoublePressActionOff NOTIFY doublePressActionOffChanged)
    QString getDoublePressActionOff();
    void setDoublePressActionOff(const QString &action);

    Q_PROPERTY(QString shortPressActionOn READ getShortPressActionOn WRITE setShortPressActionOn NOTIFY shortPressActionOnChanged)
    QString getShortPressActionOn();
    void setShortPressActionOn(const QString &action);

    Q_PROPERTY(QString shortPressActionOff READ getShortPressActionOff WRITE setShortPressActionOff NOTIFY shortPressActionOffChanged)
    QString getShortPressActionOff();
    void setShortPressActionOff(const QString &action);

    Q_PROPERTY(bool shouldRestartLipstick READ shouldRestartLipstick NOTIFY shouldRestartLipstickChanged)
    bool shouldRestartLipstick();

    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    QString version() const;

    Q_PROPERTY(QString bannerPath READ bannerPath NOTIFY bannerPathChanged)
    QString bannerPath() const;

    Q_INVOKABLE void checkActivation(const QString &code);
    Q_INVOKABLE bool checkAutostart();
    Q_INVOKABLE void setAutostart(bool enabled);
    Q_INVOKABLE void doRestartLipstick();

    Q_INVOKABLE QString readDesktopName(const QString &path) const;

private:
    QDBusInterface *iface;
    QNetworkAccessManager *nam;
    QString _version;
    QString _bannerPath;

private slots:
    void onActivationReply();
    void checkVersion();
    void createInterface();

signals:
    void versionChanged();
    void bannerPathChanged();

    void longPressDelayChanged();
    void longPressActionOnChanged();
    void longPressActionOffChanged();
    void doublePressDelayChanged();
    void doublePressActionOnChanged();
    void doublePressActionOffChanged();
    void shortPressActionOnChanged();
    void shortPressActionOffChanged();
    void shouldRestartLipstickChanged();
};

#endif // SHORTCUTSHELPER_H
