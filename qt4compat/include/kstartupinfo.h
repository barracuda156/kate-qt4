/* qt4compat: kstartupinfo.h - startup notification is an X11 concept; all no-ops */
#ifndef KDECOMPAT_KSTARTUPINFO_H
#define KDECOMPAT_KSTARTUPINFO_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtGui/QWidget>
#include <kdemacros.h>

class KDEUI_EXPORT KStartupInfoId
{
public:
    KStartupInfoId() {}
    const QByteArray &id() const { return m_id; }
    bool none() const { return true; }
    void initId(const QByteArray &id = QByteArray()) { m_id = id; }
    bool setupStartupEnv() const { return false; }
    unsigned long timestamp() const { return 0; }
private:
    QByteArray m_id;
};

class KDEUI_EXPORT KStartupInfo
{
public:
    static void appStarted() {}
    static void appStarted(const QByteArray &) {}
    static void silentClose() {}
    static void disableAutoAppStartedSending(bool = true) {}
    static void setNewStartupId(QWidget *, const QByteArray &) {}
    static void handleAutoAppStartedSending() {}
    static QByteArray createNewStartupId() { return QByteArray(); }
    static KStartupInfoId currentStartupIdEnv() { return KStartupInfoId(); }
    static void resetStartupEnv() {}
    static void setWindowStartupId(WId, const QByteArray &) {}
    static void setTimestamp(unsigned long) {}
};

#endif
