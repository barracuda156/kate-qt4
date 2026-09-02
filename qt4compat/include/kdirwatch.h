/* qt4compat: kdirwatch.h - QFileSystemWatcher based file/directory watcher */
#ifndef KDECOMPAT_KDIRWATCH_H
#define KDECOMPAT_KDIRWATCH_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <kdemacros.h>

class KDirWatchPrivate;

class KDECORE_EXPORT KDirWatch : public QObject
{
    Q_OBJECT
public:
    enum WatchMode {
        WatchDirOnly = 0,
        WatchFiles = 0x01,
        WatchSubDirs = 0x02
    };
    Q_DECLARE_FLAGS(WatchModes, WatchMode)
    enum Method { FAM, INotify, DNotify, Stat, QFSWatch };

    explicit KDirWatch(QObject *parent = 0);
    ~KDirWatch();

    void addDir(const QString &path, WatchModes watchModes = WatchDirOnly);
    void addFile(const QString &file);
    QDateTime ctime(const QString &path) const;
    void removeDir(const QString &path);
    void removeFile(const QString &file);
    bool stopDirScan(const QString &path);
    bool restartDirScan(const QString &path);
    void startScan(bool notify = false, bool skippedToo = false);
    void stopScan();
    bool isStopped();
    bool contains(const QString &path) const;
    static void statistics();
    Method internalMethod();
    static KDirWatch *self();
    static bool exists();

public Q_SLOTS:
    void setDeleted(const QString &path);
    void setDirty(const QString &path);
    void setCreated(const QString &path);

Q_SIGNALS:
    void dirty(const QString &path);
    void created(const QString &path);
    void deleted(const QString &path);

private Q_SLOTS:
    void slotFileChanged(const QString &path);
    void slotDirectoryChanged(const QString &path);
    void slotRecheck();

private:
    KDirWatchPrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KDirWatch::WatchModes)

#endif
