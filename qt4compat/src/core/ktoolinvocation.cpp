/* qt4compat: ktoolinvocation.cpp */
#include <ktoolinvocation.h>
#include <kurl.h>

#include <QtCore/QProcess>
#include <QtGui/QDesktopServices>

KToolInvocation *KToolInvocation::self()
{
    static KToolInvocation instance;
    return &instance;
}

void KToolInvocation::invokeHelp(const QString &anchor, const QString &appname, const QByteArray &)
{
    QString app = appname.isEmpty() ? QLatin1String("kate") : appname;
    QString url = QLatin1String("https://docs.kde.org/stable/en/kate/") + app + QLatin1String("/index.html");
    if (!anchor.isEmpty())
        url += QLatin1Char('#') + anchor;
    QDesktopServices::openUrl(QUrl(url));
}

void KToolInvocation::invokeMailer(const QString &address, const QString &subject, const QByteArray &)
{
    QUrl url(QLatin1String("mailto:") + address);
    if (!subject.isEmpty())
        url.addQueryItem(QLatin1String("subject"), subject);
    QDesktopServices::openUrl(url);
}

void KToolInvocation::invokeMailer(const KUrl &mailtoURL, const QByteArray &, bool)
{
    QDesktopServices::openUrl(mailtoURL);
}

void KToolInvocation::invokeBrowser(const QString &url, const QByteArray &)
{
    QDesktopServices::openUrl(QUrl(url));
}

void KToolInvocation::invokeTerminal(const QString &command, const QString &workdir, const QByteArray &)
{
#ifdef Q_OS_MAC
    QStringList args;
    args << QLatin1String("-a") << QLatin1String("Terminal");
    if (!workdir.isEmpty())
        args << workdir;
    Q_UNUSED(command)
    QProcess::startDetached(QLatin1String("open"), args);
#else
    QStringList args;
    if (!command.isEmpty())
        args << QLatin1String("-e") << command;
    QProcess::startDetached(QLatin1String("xterm"), args, workdir);
#endif
}

int KToolInvocation::startServiceByName(const QString &, const QString &, QString *error, QString *, int *, const QByteArray &, bool)
{
    if (error)
        *error = QLatin1String("services are not available");
    return -1;
}

int KToolInvocation::startServiceByDesktopPath(const QString &, const QString &, QString *error, QString *, int *, const QByteArray &, bool)
{
    if (error)
        *error = QLatin1String("services are not available");
    return -1;
}

int KToolInvocation::startServiceByDesktopName(const QString &, const QString &, QString *error, QString *, int *, const QByteArray &, bool)
{
    if (error)
        *error = QLatin1String("services are not available");
    return -1;
}

int KToolInvocation::startServiceByDesktopName(const QString &, const QStringList &, QString *error, QString *, int *, const QByteArray &, bool)
{
    if (error)
        *error = QLatin1String("services are not available");
    return -1;
}

int KToolInvocation::kdeinitExec(const QString &name, const QStringList &args, QString *error, int *pid, const QByteArray &)
{
    qint64 p = 0;
    if (!QProcess::startDetached(name, args, QString(), &p)) {
        if (error)
            *error = QLatin1String("could not start ") + name;
        return -1;
    }
    if (pid)
        *pid = int(p);
    return 0;
}

int KToolInvocation::kdeinitExecWait(const QString &name, const QStringList &args, QString *error, int *pid, const QByteArray &)
{
    const int rc = QProcess::execute(name, args);
    if (rc < 0 && error)
        *error = QLatin1String("could not start ") + name;
    if (pid)
        *pid = 0;
    return rc < 0 ? -1 : 0;
}
