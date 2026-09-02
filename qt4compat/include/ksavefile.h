/* qt4compat: ksavefile.h - atomic save through a temporary file */
#ifndef KDECOMPAT_KSAVEFILE_H
#define KDECOMPAT_KSAVEFILE_H

#include <QtCore/QFile>
#include <QtCore/QString>
#include <kdemacros.h>

class KComponentData;
class KSaveFilePrivate;

class KDECORE_EXPORT KSaveFile : public QFile
{
public:
    KSaveFile();
    explicit KSaveFile(const QString &filename);
    explicit KSaveFile(const QString &filename, const KComponentData &componentData);
    virtual ~KSaveFile();

    virtual bool open(OpenMode flags = QIODevice::ReadWrite);
    void setFileName(const QString &filename);
    QString fileName() const;
    QFile::FileError error() const;
    QString errorString() const;
    bool finalize();
    void abort();
    void setDirectWriteFallback(bool enabled);
    bool directWriteFallback() const;

    enum BackupCommand { SimpleBackup, NumberedBackup };

    static bool backupFile(const QString &filename, const QString &backupDir = QString());
    static bool simpleBackupFile(const QString &filename, const QString &backupDir = QString(), const QString &backupExtension = QLatin1String("~"));
    static bool numberedBackupFile(const QString &filename, const QString &backupDir = QString(), const QString &backupExtension = QLatin1String("~"), const uint maxBackups = 10);

private:
    KSaveFilePrivate *const d;
};

#endif
