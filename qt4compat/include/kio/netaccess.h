/* qt4compat: kio/netaccess.h - local files only */
#ifndef KDECOMPAT_KIO_NETACCESS_H
#define KDECOMPAT_KIO_NETACCESS_H

#include <QtCore/QString>
#include <kdemacros.h>
#include <kio/global.h>
#include <kurl.h>

class QWidget;
class KJob;

namespace KIO
{
    class KIO_EXPORT NetAccess
    {
    public:
        enum StatSide { SourceSide, DestinationSide };

        static bool download(const KUrl &src, QString &target, QWidget *window);
        static void removeTempFile(const QString &name);
        static bool upload(const QString &src, const KUrl &target, QWidget *window);
        static bool file_copy(const KUrl &src, const KUrl &target, QWidget *window = 0);
        static bool dircopy(const KUrl &src, const KUrl &target, QWidget *window);
        static bool dircopy(const KUrl::List &src, const KUrl &target, QWidget *window = 0);
        static bool move(const KUrl &src, const KUrl &target, QWidget *window = 0);
        static bool move(const KUrl::List &src, const KUrl &target, QWidget *window = 0);
        static bool exists(const KUrl &url, StatSide statSide, QWidget *window);
        static bool exists(const KUrl &url, bool source, QWidget *window);
        static bool stat(const KUrl &url, KIO::UDSEntry &entry, QWidget *window);
        static KUrl mostLocalUrl(const KUrl &url, QWidget *window);
        static bool del(const KUrl &url, QWidget *window);
        static bool mkdir(const KUrl &url, QWidget *window, int permissions = -1);
        static bool synchronousRun(KIO::Job *job, QWidget *window, QByteArray *data = 0, KUrl *finalURL = 0,
                                   QMap<QString, QString> *metaData = 0);
        static QString mimetype(const KUrl &url, QWidget *window);
        static QString lastErrorString();
        static int lastError();

    private:
        NetAccess();
    };
}

#endif
