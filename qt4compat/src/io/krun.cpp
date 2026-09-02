/* qt4compat: krun.cpp */
#include <krun.h>

#include <kshell.h>

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtGui/QDesktopServices>

class KRunPrivate
{
public:
    KRunPrivate() : error(false), finished(false), autoDelete(true), runExecutables(true) {}
    KUrl url;
    bool error;
    bool finished;
    bool autoDelete;
    bool runExecutables;
    QString suggestedFileName;
};

KRun::KRun(const KUrl &url, QWidget *, mode_t, bool, bool, const QByteArray &)
    : QObject(0), d(new KRunPrivate)
{
    d->url = url;
    QTimer::singleShot(0, this, SLOT(slotStart()));
}

KRun::~KRun()
{
    delete d;
}

void KRun::slotStart()
{
    d->error = !runUrl(d->url, QString(), 0, false, d->runExecutables, d->suggestedFileName);
    d->finished = true;
    if (d->error)
        emit error();
    emit finished();
    if (d->autoDelete)
        deleteLater();
}

void KRun::abort()
{
    d->finished = true;
}

bool KRun::hasError() const { return d->error; }
bool KRun::hasFinished() const { return d->finished; }
bool KRun::autoDelete() const { return d->autoDelete; }
void KRun::setAutoDelete(bool b) { d->autoDelete = b; }
void KRun::setPreferredService(const QString &) {}
void KRun::setRunExecutables(bool b) { d->runExecutables = b; }
void KRun::setEnableExternalBrowser(bool) {}
void KRun::setSuggestedFileName(const QString &fileName) { d->suggestedFileName = fileName; }
QString KRun::suggestedFileName() const { return d->suggestedFileName; }
KUrl KRun::url() const { return d->url; }

bool KRun::run(const KService &service, const KUrl::List &urls, QWidget *window, bool, const QString &, const QByteArray &)
{
    if (service.exec().isEmpty()) {
        bool ok = !urls.isEmpty();
        foreach (const KUrl &u, urls)
            ok = runUrl(u, QString(), window) && ok;
        return ok;
    }
    return run(service.exec(), urls, window, service.name(), service.icon());
}

bool KRun::run(const QString &exec, const KUrl::List &urls, QWidget *, const QString &, const QString &, const QByteArray &)
{
    KShell::Errors err = KShell::NoError;
    QStringList args = KShell::splitArgs(exec, KShell::AbortOnMeta, &err);
    if (err != KShell::NoError || args.isEmpty())
        return false;
    const QString program = args.takeFirst();
    QStringList expanded;
    bool usedUrls = false;
    foreach (const QString &a, args) {
        if (a == QLatin1String("%f") || a == QLatin1String("%u")) {
            if (!urls.isEmpty())
                expanded << (urls.first().isLocalFile() ? urls.first().toLocalFile() : urls.first().url());
            usedUrls = true;
        } else if (a == QLatin1String("%F") || a == QLatin1String("%U")) {
            foreach (const KUrl &u, urls)
                expanded << (u.isLocalFile() ? u.toLocalFile() : u.url());
            usedUrls = true;
        } else if (a.startsWith(QLatin1Char('%')) && a.length() == 2) {
            /* other field codes are dropped */
        } else {
            expanded << a;
        }
    }
    if (!usedUrls) {
        foreach (const KUrl &u, urls)
            expanded << (u.isLocalFile() ? u.toLocalFile() : u.url());
    }
    return QProcess::startDetached(program, expanded);
}

bool KRun::runUrl(const KUrl &url, const QString &mimetype, QWidget *, bool, bool runExecutables, const QString &, const QByteArray &)
{
    if (url.isLocalFile() && runExecutables && isExecutable(mimetype)) {
        QFileInfo fi(url.toLocalFile());
        if (fi.isExecutable() && !fi.isDir())
            return QProcess::startDetached(url.toLocalFile());
    }
    return QDesktopServices::openUrl(url);
}

bool KRun::runCommand(const QString &cmd, QWidget *window)
{
    return runCommand(cmd, window, QString());
}

bool KRun::runCommand(const QString &cmd, QWidget *, const QString &workingDirectory)
{
    KShell::Errors err = KShell::NoError;
    QStringList args = KShell::splitArgs(cmd, KShell::AbortOnMeta, &err);
    if (err != KShell::NoError || args.isEmpty()) {
        /* shell meta characters: hand the line to /bin/sh */
        return QProcess::startDetached(QLatin1String("/bin/sh"), QStringList() << QLatin1String("-c") << cmd, workingDirectory);
    }
    const QString program = args.takeFirst();
    return QProcess::startDetached(program, args, workingDirectory);
}

bool KRun::runCommand(const QString &cmd, const QString &, const QString &, QWidget *window, const QByteArray &)
{
    return runCommand(cmd, window, QString());
}

bool KRun::runCommand(const QString &cmd, const QString &, const QString &, QWidget *window, const QByteArray &, const QString &workingDirectory)
{
    return runCommand(cmd, window, workingDirectory);
}

bool KRun::displayOpenWithDialog(const KUrl::List &, QWidget *, bool, const QString &, const QByteArray &)
{
    return false;
}

void KRun::shellQuote(QString &str)
{
    str = KShell::quoteArg(str);
}

QStringList KRun::processDesktopExec(const KService &service, const KUrl::List &urls, bool, const QString &)
{
    QStringList result;
    result << service.exec();
    foreach (const KUrl &u, urls)
        result << (u.isLocalFile() ? u.toLocalFile() : u.url());
    return result;
}

QString KRun::binaryName(const QString &execLine, bool removePath)
{
    KShell::Errors err = KShell::NoError;
    const QStringList args = KShell::splitArgs(execLine, KShell::AbortOnMeta, &err);
    if (args.isEmpty())
        return QString();
    const QString bin = args.first();
    return removePath ? bin.section(QLatin1Char('/'), -1) : bin;
}

bool KRun::isExecutable(const QString &serviceType)
{
    return serviceType == QLatin1String("application/x-desktop")
        || serviceType == QLatin1String("application/x-executable")
        || serviceType == QLatin1String("application/x-ms-dos-executable")
        || serviceType == QLatin1String("application/x-shellscript")
        || serviceType == QLatin1String("application/x-mach-binary");
}

bool KRun::isExecutableFile(const KUrl &url, const QString &mimetype)
{
    if (!url.isLocalFile())
        return false;
    QFileInfo fi(url.toLocalFile());
    return fi.isExecutable() && !fi.isDir() && isExecutable(mimetype);
}
