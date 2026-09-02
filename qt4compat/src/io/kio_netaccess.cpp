/* qt4compat: kio_netaccess.cpp - KIO::NetAccess for local files */
#include <kio/netaccess.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <sys/stat.h>

namespace {
static QString s_lastError;
static int s_lastErrorCode = 0;

static bool localOnly(const KUrl &url)
{
    if (url.isLocalFile())
        return true;
    s_lastErrorCode = KIO::ERR_UNSUPPORTED_PROTOCOL;
    s_lastError = QLatin1String("Only local files are supported in this build: ") + url.prettyUrl();
    return false;
}
}

QString KIO::convertSize(filesize_t size)
{
    const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    double s = double(size);
    int unit = 0;
    while (s >= 1024.0 && unit < 4) {
        s /= 1024.0;
        ++unit;
    }
    return QString::number(s, 'f', unit == 0 ? 0 : 1) + QLatin1Char(' ') + QLatin1String(units[unit]);
}

QString KIO::buildErrorString(int errorCode, const QString &errorText)
{
    switch (errorCode) {
    case ERR_CANNOT_OPEN_FOR_READING: return QLatin1String("Could not read ") + errorText;
    case ERR_CANNOT_OPEN_FOR_WRITING: return QLatin1String("Could not write to ") + errorText;
    case ERR_DOES_NOT_EXIST: return QLatin1String("The file or folder ") + errorText + QLatin1String(" does not exist.");
    case ERR_ACCESS_DENIED: return QLatin1String("Access denied to ") + errorText;
    case ERR_UNSUPPORTED_PROTOCOL: return QLatin1String("Unsupported protocol: ") + errorText;
    default: return errorText;
    }
}

bool KIO::NetAccess::download(const KUrl &src, QString &target, QWidget *)
{
    if (!localOnly(src))
        return false;
    target = src.toLocalFile();
    return QFile::exists(target);
}

void KIO::NetAccess::removeTempFile(const QString &)
{
    /* download() never creates temporary files */
}

bool KIO::NetAccess::upload(const QString &src, const KUrl &target, QWidget *)
{
    if (!localOnly(target))
        return false;
    QFile::remove(target.toLocalFile());
    return QFile::copy(src, target.toLocalFile());
}

bool KIO::NetAccess::file_copy(const KUrl &src, const KUrl &target, QWidget *)
{
    if (!localOnly(src) || !localOnly(target))
        return false;
    QFile::remove(target.toLocalFile());
    return QFile::copy(src.toLocalFile(), target.toLocalFile());
}

bool KIO::NetAccess::dircopy(const KUrl &, const KUrl &, QWidget *)
{
    s_lastError = QLatin1String("Directory copy is not supported in this build.");
    return false;
}

bool KIO::NetAccess::dircopy(const KUrl::List &, const KUrl &, QWidget *)
{
    s_lastError = QLatin1String("Directory copy is not supported in this build.");
    return false;
}

bool KIO::NetAccess::move(const KUrl &src, const KUrl &target, QWidget *)
{
    if (!localOnly(src) || !localOnly(target))
        return false;
    return QFile::rename(src.toLocalFile(), target.toLocalFile());
}

bool KIO::NetAccess::move(const KUrl::List &src, const KUrl &target, QWidget *window)
{
    foreach (const KUrl &u, src) {
        KUrl dest(target);
        dest.addPath(u.fileName());
        if (!move(u, dest, window))
            return false;
    }
    return true;
}

bool KIO::NetAccess::exists(const KUrl &url, StatSide, QWidget *)
{
    if (!localOnly(url))
        return false;
    return QFile::exists(url.toLocalFile());
}

bool KIO::NetAccess::exists(const KUrl &url, bool, QWidget *)
{
    if (!localOnly(url))
        return false;
    return QFile::exists(url.toLocalFile());
}

bool KIO::NetAccess::stat(const KUrl &url, KIO::UDSEntry &entry, QWidget *)
{
    if (!localOnly(url))
        return false;
    struct stat buf;
    if (::stat(QFile::encodeName(url.toLocalFile()).constData(), &buf) != 0) {
        s_lastErrorCode = ERR_DOES_NOT_EXIST;
        s_lastError = url.toLocalFile();
        return false;
    }
    entry.clear();
    entry.insert(UDSEntry::UDS_NAME, url.fileName());
    entry.insert(UDSEntry::UDS_LOCAL_PATH, url.toLocalFile());
    entry.insert(UDSEntry::UDS_FILE_TYPE, (long long)(buf.st_mode & S_IFMT));
    entry.insert(UDSEntry::UDS_ACCESS, (long long)(buf.st_mode & 07777));
    entry.insert(UDSEntry::UDS_SIZE, (long long)buf.st_size);
    entry.insert(UDSEntry::UDS_MODIFICATION_TIME, (long long)buf.st_mtime);
    entry.insert(UDSEntry::UDS_ACCESS_TIME, (long long)buf.st_atime);
    return true;
}

KUrl KIO::NetAccess::mostLocalUrl(const KUrl &url, QWidget *)
{
    return url;
}

bool KIO::NetAccess::del(const KUrl &url, QWidget *)
{
    if (!localOnly(url))
        return false;
    const QString path = url.toLocalFile();
    if (QFileInfo(path).isDir())
        return QDir().rmdir(path);
    return QFile::remove(path);
}

bool KIO::NetAccess::mkdir(const KUrl &url, QWidget *, int)
{
    if (!localOnly(url))
        return false;
    return QDir().mkpath(url.toLocalFile());
}

bool KIO::NetAccess::synchronousRun(KIO::Job *, QWidget *, QByteArray *, KUrl *, QMap<QString, QString> *)
{
    s_lastError = QLatin1String("KIO jobs are not available in this build.");
    return false;
}

QString KIO::NetAccess::mimetype(const KUrl &url, QWidget *)
{
    Q_UNUSED(url)
    return QString();
}

QString KIO::NetAccess::lastErrorString()
{
    return s_lastError;
}

int KIO::NetAccess::lastError()
{
    return s_lastErrorCode;
}
