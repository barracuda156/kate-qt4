/* qt4compat: ksavefile.cpp */
#include <ksavefile.h>

#include <kde_file.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryFile>

#include <cerrno>
#include <cstring>

class KSaveFilePrivate
{
public:
    KSaveFilePrivate() : error(QFile::NoError), wasFinalized(false), tempFile(0), directWriteFallback(false), directWrite(false) {}
    QString realFileName;
    QString tempFileName;
    QFile::FileError error;
    QString errorString;
    bool wasFinalized;
    QTemporaryFile *tempFile;
    bool directWriteFallback;
    bool directWrite;
};

KSaveFile::KSaveFile()
    : QFile(), d(new KSaveFilePrivate)
{
}

KSaveFile::KSaveFile(const QString &filename)
    : QFile(), d(new KSaveFilePrivate)
{
    KSaveFile::setFileName(filename);
}

KSaveFile::KSaveFile(const QString &filename, const KComponentData &)
    : QFile(), d(new KSaveFilePrivate)
{
    KSaveFile::setFileName(filename);
}

KSaveFile::~KSaveFile()
{
    if (!d->wasFinalized)
        finalize();
    delete d->tempFile;
    delete d;
}

bool KSaveFile::open(OpenMode flags)
{
    if (isOpen())
        return false;
    if (d->realFileName.isEmpty()) {
        d->error = QFile::OpenError;
        d->errorString = QLatin1String("No target filename has been given.");
        return false;
    }
    if (!(flags & QIODevice::WriteOnly)) {
        flags |= QIODevice::WriteOnly;
    }

    const QFileInfo fi(d->realFileName);
    const QString dir = fi.absolutePath();
    d->directWrite = false;
    if (!QFileInfo(dir).isWritable()) {
        if (d->directWriteFallback && fi.exists() && fi.isWritable()) {
            /* cannot create a temporary file next to the target: write in place */
            d->directWrite = true;
            d->tempFileName.clear();
            QFile::setFileName(d->realFileName);
            if (!QFile::open(flags | QIODevice::Truncate)) {
                d->error = QFile::error();
                d->errorString = QFile::errorString();
                return false;
            }
            d->error = QFile::NoError;
            d->errorString.clear();
            d->wasFinalized = false;
            return true;
        }
        d->error = QFile::PermissionsError;
        d->errorString = QLatin1String("Insufficient permissions in target directory.");
        return false;
    }
    if (fi.exists() && !fi.isWritable()) {
        d->error = QFile::PermissionsError;
        d->errorString = QLatin1String("Target file is not writable.");
        return false;
    }

    delete d->tempFile;
    d->tempFile = new QTemporaryFile(dir + QLatin1Char('/') + fi.fileName() + QLatin1String(".XXXXXX"));
    if (!d->tempFile->open()) {
        d->error = d->tempFile->error();
        d->errorString = d->tempFile->errorString();
        delete d->tempFile;
        d->tempFile = 0;
        return false;
    }
    d->tempFileName = d->tempFile->fileName();
    d->tempFile->close();

    QFile::setFileName(d->tempFileName);
    if (!QFile::open(flags)) {
        d->error = QFile::error();
        d->errorString = QFile::errorString();
        return false;
    }
    /* keep permissions of an existing target */
    if (fi.exists())
        QFile::setPermissions(fi.permissions());
    d->error = QFile::NoError;
    d->errorString.clear();
    d->wasFinalized = false;
    return true;
}

void KSaveFile::setFileName(const QString &filename)
{
    d->realFileName = QFileInfo(filename).absoluteFilePath();
    QFile::setFileName(QString());
}

QString KSaveFile::fileName() const
{
    return d->realFileName;
}

QFile::FileError KSaveFile::error() const
{
    if (d->error != QFile::NoError)
        return d->error;
    return QFile::error();
}

QString KSaveFile::errorString() const
{
    if (!d->errorString.isEmpty())
        return d->errorString;
    return QFile::errorString();
}

bool KSaveFile::finalize()
{
    if (d->wasFinalized)
        return false;
    d->wasFinalized = true;
    if (d->directWrite) {
        if (isOpen())
            close();
        return d->error == QFile::NoError && QFile::error() == QFile::NoError;
    }
    if (d->tempFileName.isEmpty())
        return false;
    if (isOpen())
        close();
    if (d->error != QFile::NoError) {
        QFile::remove(d->tempFileName);
        return false;
    }
    if (KDE_rename(QFile::encodeName(d->tempFileName).constData(), QFile::encodeName(d->realFileName).constData()) != 0) {
        d->error = QFile::RenameError;
        d->errorString = QString::fromLocal8Bit(::strerror(errno));
        QFile::remove(d->tempFileName);
        return false;
    }
    delete d->tempFile;
    d->tempFile = 0;
    d->tempFileName.clear();
    return true;
}

void KSaveFile::abort()
{
    close();
    if (!d->tempFileName.isEmpty()) {
        QFile::remove(d->tempFileName);
        d->tempFileName.clear();
    }
    delete d->tempFile;
    d->tempFile = 0;
    d->wasFinalized = true;
}

void KSaveFile::setDirectWriteFallback(bool enabled)
{
    d->directWriteFallback = enabled;
}

bool KSaveFile::directWriteFallback() const
{
    return d->directWriteFallback;
}

bool KSaveFile::backupFile(const QString &filename, const QString &backupDir)
{
    return simpleBackupFile(filename, backupDir, QLatin1String("~"));
}

bool KSaveFile::simpleBackupFile(const QString &filename, const QString &backupDir, const QString &backupExtension)
{
    QString backup;
    if (backupDir.isEmpty()) {
        backup = filename + backupExtension;
    } else {
        QString dir = backupDir;
        if (!dir.endsWith(QLatin1Char('/')))
            dir += QLatin1Char('/');
        backup = dir + QFileInfo(filename).fileName() + backupExtension;
    }
    QFile::remove(backup);
    return QFile::copy(filename, backup);
}

bool KSaveFile::numberedBackupFile(const QString &filename, const QString &backupDir, const QString &backupExtension, const uint maxBackups)
{
    QFileInfo fi(filename);
    QString dir = backupDir.isEmpty() ? fi.absolutePath() : backupDir;
    if (!dir.endsWith(QLatin1Char('/')))
        dir += QLatin1Char('/');
    const QString base = fi.fileName();
    /* rotate existing backups: name.N~ -> name.N+1~ */
    for (int i = int(maxBackups); i >= 1; --i) {
        const QString older = dir + base + QLatin1Char('.') + QString::number(i) + backupExtension;
        if (!QFile::exists(older))
            continue;
        if (uint(i) >= maxBackups) {
            QFile::remove(older);
        } else {
            const QString newer = dir + base + QLatin1Char('.') + QString::number(i + 1) + backupExtension;
            QFile::remove(newer);
            QFile::rename(older, newer);
        }
    }
    return QFile::copy(filename, dir + base + QLatin1String(".1") + backupExtension);
}
