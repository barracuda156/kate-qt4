/* qt4compat: kfileitem.cpp */
#include <kfileitem.h>

#include <kiconloader.h>

#include <QtCore/QFileInfo>

#include <sys/stat.h>

class KFileItemPrivate
{
public:
    KFileItemPrivate() : null(true), mode(0), permissions(0), size(0), isLink(false), exists(false), readable(false), writable(false) {}
    bool null;
    KUrl url;
    QString name;
    QString mimeType;
    mode_t mode;
    mode_t permissions;
    KIO::filesize_t size;
    QDateTime mtime, atime, ctime;
    bool isLink;
    bool exists;
    bool readable;
    bool writable;
    QString linkDest;
    QString user, group;
    KIO::UDSEntry entry;

    void fillFromLocal();
};

void KFileItemPrivate::fillFromLocal()
{
    if (!url.isLocalFile())
        return;
    const QString path = url.toLocalFile();
    QFileInfo fi(path);
    exists = fi.exists();
    if (!exists && !fi.isSymLink())
        return;
    struct stat buf;
    if (::lstat(QFile::encodeName(path).constData(), &buf) == 0) {
        isLink = S_ISLNK(buf.st_mode);
        if (isLink) {
            linkDest = fi.symLinkTarget();
            ::stat(QFile::encodeName(path).constData(), &buf);
        }
        mode = buf.st_mode & S_IFMT;
        permissions = buf.st_mode & 07777;
        size = buf.st_size;
        mtime = QDateTime::fromTime_t(buf.st_mtime);
        atime = QDateTime::fromTime_t(buf.st_atime);
        ctime = QDateTime::fromTime_t(buf.st_ctime);
    }
    readable = fi.isReadable();
    writable = fi.isWritable();
    user = fi.owner();
    group = fi.group();
    if (name.isEmpty())
        name = fi.fileName();
}

KFileItem::KFileItem()
    : d(new KFileItemPrivate)
{
}

KFileItem::KFileItem(const KIO::UDSEntry &entry, const KUrl &itemOrDirUrl, bool, bool urlIsDirectory)
    : d(new KFileItemPrivate)
{
    d->null = false;
    d->entry = entry;
    d->url = itemOrDirUrl;
    const QString name = entry.stringValue(KIO::UDSEntry::UDS_NAME);
    if (urlIsDirectory && !name.isEmpty())
        d->url.addPath(name);
    d->name = name.isEmpty() ? d->url.fileName() : name;
    d->fillFromLocal();
    if (!d->exists) {
        d->mode = entry.numberValue(KIO::UDSEntry::UDS_FILE_TYPE, 0);
        d->permissions = entry.numberValue(KIO::UDSEntry::UDS_ACCESS, 0);
        d->size = entry.numberValue(KIO::UDSEntry::UDS_SIZE, 0);
    }
}

KFileItem::KFileItem(mode_t mode, mode_t permissions, const KUrl &url, bool)
    : d(new KFileItemPrivate)
{
    d->null = false;
    d->url = url;
    d->name = url.fileName();
    d->fillFromLocal();
    if (mode != mode_t(Unknown) && !d->exists)
        d->mode = mode;
    if (permissions != mode_t(Unknown) && !d->exists)
        d->permissions = permissions;
}

KFileItem::KFileItem(const KUrl &url, const QString &mimeType, mode_t mode)
    : d(new KFileItemPrivate)
{
    d->null = false;
    d->url = url;
    d->name = url.fileName();
    d->mimeType = mimeType;
    d->fillFromLocal();
    if (mode != mode_t(Unknown) && !d->exists)
        d->mode = mode;
}

KFileItem::KFileItem(const KFileItem &other)
    : d(new KFileItemPrivate(*other.d))
{
}

KFileItem::~KFileItem()
{
    delete d;
}

KFileItem &KFileItem::operator=(const KFileItem &other)
{
    if (this != &other)
        *d = *other.d;
    return *this;
}

bool KFileItem::operator==(const KFileItem &other) const
{
    if (d->null || other.d->null)
        return d->null == other.d->null;
    return d->url == other.d->url;
}

void KFileItem::refresh()
{
    d->fillFromLocal();
}

void KFileItem::refreshMimeType()
{
    d->mimeType.clear();
}

KUrl KFileItem::url() const { return d->url; }

void KFileItem::setUrl(const KUrl &url)
{
    d->url = url;
    d->name = url.fileName();
    d->null = false;
    d->fillFromLocal();
}

void KFileItem::setName(const QString &name) { d->name = name; }
mode_t KFileItem::permissions() const { return d->permissions; }

QString KFileItem::permissionsString() const
{
    QString s(QLatin1String("----------"));
    if (S_ISDIR(d->mode)) s[0] = QLatin1Char('d');
    else if (d->isLink) s[0] = QLatin1Char('l');
    const char *bits = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i)
        if (d->permissions & (0400 >> i))
            s[i + 1] = QLatin1Char(bits[i]);
    return s;
}

mode_t KFileItem::mode() const { return d->mode; }
QString KFileItem::user() const { return d->user; }
QString KFileItem::group() const { return d->group; }
bool KFileItem::isLink() const { return d->isLink; }
bool KFileItem::isDir() const { return S_ISDIR(d->mode); }
bool KFileItem::isFile() const { return !S_ISDIR(d->mode); }
bool KFileItem::isReadable() const { return d->readable; }
bool KFileItem::isWritable() const { return d->writable; }
bool KFileItem::isHidden() const { return d->name.startsWith(QLatin1Char('.')); }
bool KFileItem::isSlow() const { return !d->url.isLocalFile(); }
bool KFileItem::isDesktopFile() const { return d->name.endsWith(QLatin1String(".desktop")); }
QString KFileItem::linkDest() const { return d->linkDest; }
KUrl KFileItem::targetUrl() const { return d->isLink && !d->linkDest.isEmpty() ? KUrl::fromPath(d->linkDest) : d->url; }
QString KFileItem::localPath() const { return d->url.isLocalFile() ? d->url.toLocalFile() : QString(); }
KIO::filesize_t KFileItem::size() const { return d->size; }

QDateTime KFileItem::time(FileTimes which) const
{
    switch (which) {
    case AccessTime: return d->atime;
    case CreationTime: return d->ctime;
    default: return d->mtime;
    }
}

QString KFileItem::timeString(FileTimes which) const
{
    return time(which).toString(Qt::DefaultLocaleShortDate);
}

bool KFileItem::isLocalFile() const { return d->url.isLocalFile(); }
QString KFileItem::text() const { return d->name; }
QString KFileItem::name(bool lowerCase) const { return lowerCase ? d->name.toLower() : d->name; }

QString KFileItem::mimetype() const
{
    return determineMimeType()->name();
}

KMimeType::Ptr KFileItem::determineMimeType() const
{
    if (!d->mimeType.isEmpty()) {
        KMimeType::Ptr p = KMimeType::mimeType(d->mimeType);
        if (p)
            return p;
    }
    KMimeType::Ptr p = KMimeType::findByUrl(d->url, d->mode, d->url.isLocalFile(), false);
    if (p)
        d->mimeType = p->name();
    return p ? p : KMimeType::defaultMimeTypePtr();
}

KMimeType::Ptr KFileItem::mimeTypePtr() const
{
    if (!d->mimeType.isEmpty()) {
        KMimeType::Ptr p = KMimeType::mimeType(d->mimeType);
        if (p)
            return p;
    }
    KMimeType::Ptr p = KMimeType::findByUrl(d->url, d->mode, d->url.isLocalFile(), true);
    return p ? p : KMimeType::defaultMimeTypePtr();
}

bool KFileItem::isMimeTypeKnown() const { return !d->mimeType.isEmpty(); }
QString KFileItem::mimeComment() const { return mimeTypePtr()->comment(); }
QString KFileItem::iconName() const { return mimeTypePtr()->iconName(d->url); }

QPixmap KFileItem::pixmap(int size, int state) const
{
    return KIconLoader::global()->loadIcon(iconName(), KIconLoader::Desktop, size, state);
}

QStringList KFileItem::overlays() const { return QStringList(); }
QString KFileItem::comment() const { return mimeComment(); }

QString KFileItem::getStatusBarInfo() const
{
    if (isDir())
        return d->name + QLatin1String(" (") + mimeComment() + QLatin1Char(')');
    return d->name + QLatin1String(" (") + mimeComment() + QLatin1String(", ") + KIO::convertSize(d->size) + QLatin1Char(')');
}

KIO::UDSEntry KFileItem::entry() const { return d->entry; }
bool KFileItem::isRegularFile() const { return S_ISREG(d->mode); }
bool KFileItem::cmp(const KFileItem &item) const { return *this == item && d->size == item.d->size && d->mtime == item.d->mtime; }

KFileItem::operator QVariant() const
{
    return qVariantFromValue(*this);
}

KUrl KFileItem::mostLocalUrl(bool &local) const
{
    local = d->url.isLocalFile();
    return d->url;
}

KUrl KFileItem::mostLocalUrl() const { return d->url; }
bool KFileItem::isNull() const { return d->null; }

KFileItem KFileItemList::findByName(const QString &fileName) const
{
    for (const_iterator it = constBegin(); it != constEnd(); ++it)
        if ((*it).name() == fileName)
            return *it;
    return KFileItem();
}

KFileItem KFileItemList::findByUrl(const KUrl &url) const
{
    for (const_iterator it = constBegin(); it != constEnd(); ++it)
        if ((*it).url() == url)
            return *it;
    return KFileItem();
}

KUrl::List KFileItemList::urlList() const
{
    KUrl::List list;
    for (const_iterator it = constBegin(); it != constEnd(); ++it)
        list << (*it).url();
    return list;
}

KUrl::List KFileItemList::targetUrlList() const
{
    KUrl::List list;
    for (const_iterator it = constBegin(); it != constEnd(); ++it)
        list << (*it).targetUrl();
    return list;
}
