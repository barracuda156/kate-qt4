/* qt4compat: kfileitem.h - file metadata from the local file system */
#ifndef KDECOMPAT_KFILEITEM_H
#define KDECOMPAT_KFILEITEM_H

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QPixmap>
#include <kdemacros.h>
#include <kio/global.h>
#include <kio/udsentry.h>
#include <kmimetype.h>
#include <kurl.h>

#include <sys/types.h>

class KFileItemPrivate;

class KIO_EXPORT KFileItem
{
public:
    enum { Unknown = static_cast<mode_t>(-1) };
    enum FileTimes { ModificationTime = 0, AccessTime = 1, CreationTime = 2 };

    KFileItem();
    KFileItem(const KIO::UDSEntry &entry, const KUrl &itemOrDirUrl, bool delayedMimeTypes = false, bool urlIsDirectory = false);
    KFileItem(mode_t mode, mode_t permissions, const KUrl &url, bool delayedMimeTypes = false);
    KFileItem(const KUrl &url, const QString &mimeType, mode_t mode);
    KFileItem(const KFileItem &other);
    ~KFileItem();
    KFileItem &operator=(const KFileItem &other);
    bool operator==(const KFileItem &other) const;
    bool operator!=(const KFileItem &other) const { return !operator==(other); }

    void refresh();
    void refreshMimeType();
    KUrl url() const;
    void setUrl(const KUrl &url);
    void setName(const QString &name);
    mode_t permissions() const;
    QString permissionsString() const;
    mode_t mode() const;
    QString user() const;
    QString group() const;
    bool isLink() const;
    bool isDir() const;
    bool isFile() const;
    bool isReadable() const;
    bool isWritable() const;
    bool isHidden() const;
    bool isSlow() const;
    bool isDesktopFile() const;
    QString linkDest() const;
    KUrl targetUrl() const;
    QString localPath() const;
    KIO::filesize_t size() const;
    QDateTime time(FileTimes which) const;
    QString timeString(FileTimes which = ModificationTime) const;
    bool isLocalFile() const;
    QString text() const;
    QString name(bool lowerCase = false) const;
    QString mimetype() const;
    KMimeType::Ptr determineMimeType() const;
    KMimeType::Ptr mimeTypePtr() const;
    bool isMimeTypeKnown() const;
    QString mimeComment() const;
    QString iconName() const;
    QPixmap pixmap(int size, int state = 0) const;
    QStringList overlays() const;
    QString comment() const;
    QString getStatusBarInfo() const;
    KIO::UDSEntry entry() const;
    bool isRegularFile() const;
    bool cmp(const KFileItem &item) const;
    operator QVariant() const;
    KUrl mostLocalUrl(bool &local) const;
    KUrl mostLocalUrl() const;
    bool isNull() const;

private:
    KFileItemPrivate *d;
};

class KIO_EXPORT KFileItemList : public QList<KFileItem>
{
public:
    KFileItemList() {}
    KFileItemList(const QList<KFileItem> &items) : QList<KFileItem>(items) {}
    KFileItem findByName(const QString &fileName) const;
    KFileItem findByUrl(const KUrl &url) const;
    KUrl::List urlList() const;
    KUrl::List targetUrlList() const;
};

Q_DECLARE_METATYPE(KFileItem)
Q_DECLARE_METATYPE(KFileItemList)

#endif
