/* qt4compat: kio/global.h - enums and forward declarations only. There is no
 * KIO in this build; jobs cannot be created. */
#ifndef KDECOMPAT_KIO_GLOBAL_H
#define KDECOMPAT_KIO_GLOBAL_H

#include <QtCore/QFlags>
#include <QtCore/QString>
#include <kdemacros.h>
#include <kio/udsentry.h>

class KUrl;

namespace KIO
{
    typedef qulonglong filesize_t;
    typedef qulonglong fileoffset_t;

    enum JobFlag { DefaultFlags = 0, HideProgressInfo = 1, Resume = 2, Overwrite = 4 };
    Q_DECLARE_FLAGS(JobFlags, JobFlag)

    enum LoadType { Reload, NoReload };

    enum Error {
        ERR_CANNOT_OPEN_FOR_READING = 1,
        ERR_CANNOT_OPEN_FOR_WRITING = 2,
        ERR_CANNOT_LAUNCH_PROCESS = 3,
        ERR_INTERNAL = 4,
        ERR_MALFORMED_URL = 5,
        ERR_UNSUPPORTED_PROTOCOL = 6,
        ERR_NO_SOURCE_PROTOCOL = 7,
        ERR_UNSUPPORTED_ACTION = 8,
        ERR_IS_DIRECTORY = 9,
        ERR_IS_FILE = 10,
        ERR_DOES_NOT_EXIST = 11,
        ERR_FILE_ALREADY_EXIST = 12,
        ERR_DIR_ALREADY_EXIST = 13,
        ERR_UNKNOWN_HOST = 14,
        ERR_ACCESS_DENIED = 15,
        ERR_WRITE_ACCESS_DENIED = 16,
        ERR_CANNOT_ENTER_DIRECTORY = 17,
        ERR_USER_CANCELED = 25,
        ERR_COULD_NOT_READ = 41,
        ERR_COULD_NOT_WRITE = 42,
        ERR_UNKNOWN = 71
    };

    class Job;
    class SimpleJob;
    class TransferJob;
    class FileCopyJob;
    class CopyJob;
    class DeleteJob;
    class ListJob;
    class StatJob;
    class MkdirJob;
    class StoredTransferJob;

    KIO_EXPORT QString convertSize(filesize_t size);
    KIO_EXPORT QString buildErrorString(int errorCode, const QString &errorText);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KIO::JobFlags)

#endif
