/* qt4compat: kdirwatch.cpp */
#include <kdirwatch.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QTimer>

class KDirWatchPrivate
{
public:
    KDirWatchPrivate() : watcher(0), stopped(false) {}
    QFileSystemWatcher *watcher;
    QSet<QString> files;
    QSet<QString> dirs;
    QHash<QString, QStringList> dirEntries; /* for created()/deleted() in directories */
    QSet<QString> missingFiles;              /* watched files that do not exist (yet) */
    QSet<QString> stoppedDirs;
    bool stopped;
    QTimer recheck;
};

static QString clean(const QString &p)
{
    QString c = QDir::cleanPath(p);
    while (c.length() > 1 && c.endsWith(QLatin1Char('/')))
        c.chop(1);
    return c;
}

KDirWatch::KDirWatch(QObject *parent)
    : QObject(parent), d(new KDirWatchPrivate)
{
    d->watcher = new QFileSystemWatcher(this);
    connect(d->watcher, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)));
    connect(d->watcher, SIGNAL(directoryChanged(QString)), this, SLOT(slotDirectoryChanged(QString)));
    d->recheck.setInterval(1500);
    d->recheck.setSingleShot(false);
    connect(&d->recheck, SIGNAL(timeout()), this, SLOT(slotRecheck()));
}

KDirWatch::~KDirWatch()
{
    delete d;
}

KDirWatch *KDirWatch::self()
{
    static KDirWatch *s = 0;
    if (!s)
        s = new KDirWatch;
    return s;
}

bool KDirWatch::exists()
{
    return true;
}

void KDirWatch::addDir(const QString &path, WatchModes)
{
    const QString p = clean(path);
    if (p.isEmpty() || d->dirs.contains(p))
        return;
    d->dirs.insert(p);
    if (QDir(p).exists()) {
        d->dirEntries.insert(p, QDir(p).entryList(QDir::AllEntries | QDir::NoDotAndDotDot));
        d->watcher->addPath(p);
    }
}

void KDirWatch::addFile(const QString &file)
{
    const QString p = clean(file);
    if (p.isEmpty() || d->files.contains(p))
        return;
    d->files.insert(p);
    if (QFile::exists(p)) {
        d->watcher->addPath(p);
    } else {
        d->missingFiles.insert(p);
        if (!d->recheck.isActive())
            d->recheck.start();
    }
}

QDateTime KDirWatch::ctime(const QString &path) const
{
    return QFileInfo(path).lastModified();
}

void KDirWatch::removeDir(const QString &path)
{
    const QString p = clean(path);
    if (!d->dirs.remove(p))
        return;
    d->dirEntries.remove(p);
    d->watcher->removePath(p);
}

void KDirWatch::removeFile(const QString &file)
{
    const QString p = clean(file);
    if (!d->files.remove(p))
        return;
    d->missingFiles.remove(p);
    d->watcher->removePath(p);
    if (d->missingFiles.isEmpty())
        d->recheck.stop();
}

bool KDirWatch::stopDirScan(const QString &path)
{
    const QString p = clean(path);
    if (!d->dirs.contains(p))
        return false;
    d->stoppedDirs.insert(p);
    return true;
}

bool KDirWatch::restartDirScan(const QString &path)
{
    const QString p = clean(path);
    if (!d->dirs.contains(p))
        return false;
    d->stoppedDirs.remove(p);
    if (QDir(p).exists())
        d->dirEntries.insert(p, QDir(p).entryList(QDir::AllEntries | QDir::NoDotAndDotDot));
    return true;
}

void KDirWatch::startScan(bool, bool)
{
    d->stopped = false;
}

void KDirWatch::stopScan()
{
    d->stopped = true;
}

bool KDirWatch::isStopped()
{
    return d->stopped;
}

bool KDirWatch::contains(const QString &path) const
{
    const QString p = clean(path);
    return d->files.contains(p) || d->dirs.contains(p);
}

void KDirWatch::statistics()
{
}

KDirWatch::Method KDirWatch::internalMethod()
{
    return QFSWatch;
}

void KDirWatch::setDeleted(const QString &path)
{
    if (!d->stopped)
        emit deleted(path);
}

void KDirWatch::setDirty(const QString &path)
{
    if (!d->stopped)
        emit dirty(path);
}

void KDirWatch::setCreated(const QString &path)
{
    if (!d->stopped)
        emit created(path);
}

void KDirWatch::slotFileChanged(const QString &path)
{
    const QString p = clean(path);
    if (!d->files.contains(p))
        return;
    if (QFile::exists(p)) {
        /* editors that save via rename drop the watch: re-add it */
        if (!d->watcher->files().contains(p))
            d->watcher->addPath(p);
        if (!d->stopped)
            emit dirty(p);
    } else {
        d->watcher->removePath(p);
        d->missingFiles.insert(p);
        if (!d->recheck.isActive())
            d->recheck.start();
        if (!d->stopped)
            emit deleted(p);
    }
}

void KDirWatch::slotDirectoryChanged(const QString &path)
{
    const QString p = clean(path);
    if (!d->dirs.contains(p))
        return;
    if (!QDir(p).exists()) {
        d->watcher->removePath(p);
        d->dirEntries.remove(p);
        if (!d->stopped)
            emit deleted(p);
        return;
    }
    if (d->stoppedDirs.contains(p) || d->stopped)
        return;
    const QStringList now = QDir(p).entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    const QStringList before = d->dirEntries.value(p);
    d->dirEntries.insert(p, now);
    foreach (const QString &e, now)
        if (!before.contains(e))
            emit created(p + QLatin1Char('/') + e);
    foreach (const QString &e, before)
        if (!now.contains(e))
            emit deleted(p + QLatin1Char('/') + e);
    emit dirty(p);
}

void KDirWatch::slotRecheck()
{
    QSet<QString> appeared;
    foreach (const QString &p, d->missingFiles) {
        if (QFile::exists(p))
            appeared.insert(p);
    }
    foreach (const QString &p, appeared) {
        d->missingFiles.remove(p);
        d->watcher->addPath(p);
        if (!d->stopped)
            emit created(p);
    }
    if (d->missingFiles.isEmpty())
        d->recheck.stop();
}
