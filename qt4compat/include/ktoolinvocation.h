/* qt4compat: ktoolinvocation.h */
#ifndef KDECOMPAT_KTOOLINVOCATION_H
#define KDECOMPAT_KTOOLINVOCATION_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>

class KUrl;

class KDECORE_EXPORT KToolInvocation
{
public:
    static KToolInvocation *self();

    static void invokeHelp(const QString &anchor = QString(), const QString &appname = QString(), const QByteArray &startup_id = QByteArray());
    static void invokeMailer(const QString &address, const QString &subject, const QByteArray &startup_id = QByteArray());
    static void invokeMailer(const KUrl &mailtoURL, const QByteArray &startup_id = QByteArray(), bool allowAttachments = false);
    static void invokeBrowser(const QString &url, const QByteArray &startup_id = QByteArray());
    static void invokeTerminal(const QString &command, const QString &workdir = QString(), const QByteArray &startup_id = QByteArray());
    static int startServiceByName(const QString &name, const QString &URL, QString *error = 0, QString *serviceName = 0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false);
    static int startServiceByDesktopPath(const QString &name, const QString &URL, QString *error = 0, QString *serviceName = 0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false);
    static int startServiceByDesktopName(const QString &name, const QString &URL, QString *error = 0, QString *serviceName = 0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false);
    static int startServiceByDesktopName(const QString &name, const QStringList &URLs, QString *error = 0, QString *serviceName = 0, int *pid = 0, const QByteArray &startup_id = QByteArray(), bool noWait = false);
    static int kdeinitExec(const QString &name, const QStringList &args = QStringList(), QString *error = 0, int *pid = 0, const QByteArray &startup_id = QByteArray());
    static int kdeinitExecWait(const QString &name, const QStringList &args = QStringList(), QString *error = 0, int *pid = 0, const QByteArray &startup_id = QByteArray());
};

#endif
