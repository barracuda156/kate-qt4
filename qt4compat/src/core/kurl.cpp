/* qt4compat: kurl.cpp */
#include <kurl.h>

#include <QtCore/QDir>
#include <QtCore/QMimeData>

static QString expandTilde(const QString &path)
{
    if (path == QLatin1String("~"))
        return QDir::homePath();
    if (path.startsWith(QLatin1String("~/")))
        return QDir::homePath() + path.mid(1);
    return path;
}

static QString adjust(const QString &path, KUrl::AdjustPathOption trailing)
{
    if (path.isEmpty())
        return path;
    if (trailing == KUrl::RemoveTrailingSlash) {
        if (path.length() > 1 && path.endsWith(QLatin1Char('/'))) {
            QString p = path;
            while (p.length() > 1 && p.endsWith(QLatin1Char('/')))
                p.chop(1);
            return p;
        }
        return path;
    }
    if (trailing == KUrl::AddTrailingSlash && !path.endsWith(QLatin1Char('/')))
        return path + QLatin1Char('/');
    return path;
}

/* KUrl::List */

KUrl::List::List(const KUrl &url)
{
    append(url);
}

KUrl::List::List(const QStringList &list)
{
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
        append(KUrl(*it));
}

KUrl::List::List(const QList<KUrl> &list)
    : QList<KUrl>(list)
{
}

KUrl::List::List(const QList<QUrl> &list)
{
    foreach (const QUrl &u, list)
        append(KUrl(u));
}

QStringList KUrl::List::toStringList() const
{
    return toStringList(KUrl::LeaveTrailingSlash);
}

QStringList KUrl::List::toStringList(KUrl::AdjustPathOption trailing) const
{
    QStringList lst;
    for (KUrl::List::ConstIterator it = constBegin(); it != constEnd(); ++it)
        lst.append((*it).url(trailing));
    return lst;
}

void KUrl::List::populateMimeData(QMimeData *mimeData, const KUrl::MetaDataMap &, MimeDataFlags flags) const
{
    QList<QUrl> urls;
    for (KUrl::List::ConstIterator it = constBegin(); it != constEnd(); ++it)
        urls.append(*it);
    mimeData->setUrls(urls);
    if (!(flags & KUrl::NoTextExport))
        mimeData->setText(toStringList().join(QLatin1String("\n")));
}

void KUrl::List::populateMimeData(const KUrl::List &, QMimeData *mimeData, const KUrl::MetaDataMap &metaData, MimeDataFlags flags) const
{
    populateMimeData(mimeData, metaData, flags);
}

bool KUrl::List::canDecode(const QMimeData *mimeData)
{
    return mimeData && mimeData->hasUrls();
}

QStringList KUrl::List::mimeDataTypes()
{
    return QStringList() << QLatin1String("text/uri-list");
}

KUrl::List KUrl::List::fromMimeData(const QMimeData *mimeData, KUrl::MetaDataMap *)
{
    KUrl::List result;
    if (!mimeData)
        return result;
    foreach (const QUrl &u, mimeData->urls())
        result.append(KUrl(u));
    return result;
}

KUrl::List KUrl::List::fromMimeData(const QMimeData *mimeData, DecodeOptions, KUrl::MetaDataMap *metaData)
{
    return fromMimeData(mimeData, metaData);
}

/* KUrl */

KUrl::KUrl()
    : QUrl()
{
}

void KUrl::init(const QString &str)
{
    if (str.isEmpty())
        return;
    const QChar first = str.at(0);
    if (first == QLatin1Char('/') || first == QLatin1Char('~')) {
        setPath(expandTilde(str));
    } else {
        QUrl::operator=(QUrl(str, QUrl::TolerantMode));
        if (scheme().isEmpty() && !str.startsWith(QLatin1String("//")) && !str.contains(QLatin1Char(':')) && !isEmpty()) {
            /* plain relative path: keep as a path-only URL like kdelibs did */
            QUrl::setPath(str);
        }
    }
}

KUrl::KUrl(const QString &str)
    : QUrl()
{
    init(str);
}

KUrl::KUrl(const char *str)
    : QUrl()
{
    init(QString::fromUtf8(str));
}

KUrl::KUrl(const QByteArray &str)
    : QUrl()
{
    init(QString::fromUtf8(str.constData(), str.length()));
}

KUrl::KUrl(const KUrl &u)
    : QUrl(u)
{
}

KUrl::KUrl(const QUrl &u)
    : QUrl(u)
{
}

KUrl::KUrl(const KUrl &base, const QString &relative)
    : QUrl()
{
    if (relative.isEmpty()) {
        *this = base;
        return;
    }
    if (relative.startsWith(QLatin1Char('/')) || relative.startsWith(QLatin1Char('~'))) {
        setPath(expandTilde(relative));
        return;
    }
    const QUrl rel(relative, QUrl::TolerantMode);
    if (!rel.scheme().isEmpty())
        QUrl::operator=(rel);
    else
        QUrl::operator=(base.resolved(rel));
}

KUrl::~KUrl()
{
}

KUrl &KUrl::operator=(const KUrl &u)
{
    QUrl::operator=(u);
    return *this;
}

KUrl &KUrl::operator=(const QUrl &u)
{
    QUrl::operator=(u);
    return *this;
}

KUrl &KUrl::operator=(const QString &s)
{
    QUrl::operator=(QUrl());
    init(s);
    return *this;
}

KUrl::operator QVariant() const
{
    return qVariantFromValue(*this);
}

bool KUrl::operator==(const KUrl &u) const
{
    return QUrl::operator==(u);
}

bool KUrl::operator==(const QString &s) const
{
    return QUrl::operator==(KUrl(s));
}

bool KUrl::equals(const KUrl &u, const EqualsOptions &options) const
{
    if (!isValid() || !u.isValid())
        return false;
    KUrl a(*this);
    KUrl b(u);
    if (options & CompareWithoutTrailingSlash) {
        a.adjustPath(RemoveTrailingSlash);
        b.adjustPath(RemoveTrailingSlash);
    }
    if (options & AllowEmptyPath) {
        if (a.path().isEmpty())
            a.QUrl::setPath(QLatin1String("/"));
        if (b.path().isEmpty())
            b.QUrl::setPath(QLatin1String("/"));
    }
    if (options & CompareWithoutFragment) {
        a.setFragment(QString());
        b.setFragment(QString());
    }
    return a.QUrl::operator==(b);
}

bool KUrl::isParentOf(const KUrl &u) const
{
    if (scheme() != u.scheme() || host() != u.host() || port() != u.port() || userName() != u.userName())
        return false;
    const QString a = path(AddTrailingSlash);
    const QString b = u.path(AddTrailingSlash);
    return b.startsWith(a);
}

QString KUrl::protocol() const
{
    return scheme().toLower();
}

void KUrl::setProtocol(const QString &proto)
{
    setScheme(proto);
}

QString KUrl::user() const { return userName(); }
void KUrl::setUser(const QString &user) { setUserName(user); }
bool KUrl::hasUser() const { return !userName().isEmpty(); }
QString KUrl::pass() const { return password(); }
void KUrl::setPass(const QString &pass) { setPassword(pass); }
bool KUrl::hasPass() const { return !password().isEmpty(); }
bool KUrl::hasHost() const { return !host().isEmpty(); }

QString KUrl::path(AdjustPathOption trailing) const
{
    return adjust(QUrl::path(), trailing);
}

QString KUrl::toLocalFile(AdjustPathOption trailing) const
{
    if (!isLocalFile())
        return QString();
    QString p = QUrl::toLocalFile();
    if (p.isEmpty())
        p = QUrl::path();
    return adjust(p, trailing);
}

void KUrl::setPath(const QString &_path)
{
    const QString path = expandTilde(_path);
    if (!path.isEmpty() && scheme().isEmpty())
        setScheme(QLatin1String("file"));
    QUrl::setPath(path);
}

bool KUrl::hasPath() const
{
    return !QUrl::path().isEmpty();
}

void KUrl::cleanPath(const CleanPathOptions &)
{
    const QString p = QUrl::path();
    if (p.isEmpty())
        return;
    const bool trailing = p.length() > 1 && p.endsWith(QLatin1Char('/'));
    QString cleaned = QDir::cleanPath(p);
    if (trailing && !cleaned.endsWith(QLatin1Char('/')))
        cleaned += QLatin1Char('/');
    QUrl::setPath(cleaned);
}

void KUrl::adjustPath(AdjustPathOption trailing)
{
    QUrl::setPath(adjust(QUrl::path(), trailing));
}

void KUrl::setEncodedPathAndQuery(const QString &txt)
{
    const int pos = txt.indexOf(QLatin1Char('?'));
    if (pos < 0) {
        setEncodedPath(txt);
        setEncodedQuery(QByteArray());
    } else {
        setEncodedPath(txt.left(pos));
        setEncodedQuery(txt.mid(pos + 1).toLatin1());
    }
}

void KUrl::setEncodedPath(const QString &txt)
{
    QUrl::setEncodedPath(txt.toLatin1());
    if (!txt.isEmpty() && scheme().isEmpty())
        setScheme(QLatin1String("file"));
}

QString KUrl::encodedPathAndQuery(AdjustPathOption trailing, const EncodedPathAndQueryOptions &options) const
{
    QString p = adjust(QString::fromLatin1(QUrl::encodedPath()), trailing);
    if (p.isEmpty() && (options & AvoidEmptyPath))
        p = QLatin1String("/");
    if (hasQuery())
        p += QLatin1Char('?') + QString::fromLatin1(encodedQuery());
    return p;
}

void KUrl::setQuery(const QString &query)
{
    if (query.isEmpty())
        setEncodedQuery(QByteArray());
    else
        setEncodedQuery((query.startsWith(QLatin1Char('?')) ? query.mid(1) : query).toLatin1());
}

QString KUrl::query() const
{
    if (!hasQuery())
        return QString();
    return QLatin1Char('?') + QString::fromLatin1(encodedQuery());
}

QString KUrl::ref() const { return fragment(); }
void KUrl::setRef(const QString &fragment) { setFragment(fragment); }
bool KUrl::hasRef() const { return hasFragment(); }
QString KUrl::htmlRef() const { return fragment(); }
QString KUrl::encodedHtmlRef() const { return QString::fromLatin1(encodedFragment()); }
void KUrl::setHTMLRef(const QString &ref) { setFragment(ref); }
bool KUrl::hasHTMLRef() const { return hasFragment(); }

bool KUrl::isLocalFile() const
{
    if (scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) != 0)
        return false;
    return host().isEmpty() || host() == QLatin1String("localhost");
}

void KUrl::setFileEncoding(const QString &)
{
}

QString KUrl::fileEncoding() const
{
    return QString();
}

bool KUrl::hasSubUrl() const
{
    return false;
}

void KUrl::addPath(const QString &txt)
{
    if (txt.isEmpty())
        return;
    QString p = QUrl::path();
    if (p.isEmpty() && scheme().isEmpty())
        setScheme(QLatin1String("file"));
    const bool pSlash = p.endsWith(QLatin1Char('/'));
    const bool tSlash = txt.startsWith(QLatin1Char('/'));
    if (pSlash && tSlash)
        p += txt.mid(1);
    else if (!pSlash && !tSlash && !p.isEmpty())
        p += QLatin1Char('/') + txt;
    else
        p += txt;
    QUrl::setPath(p);
}

QString KUrl::queryItem(const QString &item) const
{
    return QUrl::queryItemValue(item);
}

void KUrl::addQueryItem(const QString &item, const QString &value)
{
    QUrl::addQueryItem(item, value);
}

void KUrl::setFileName(const QString &txt)
{
    QString p = QUrl::path();
    setFragment(QString());
    setEncodedQuery(QByteArray());
    if (p.endsWith(QLatin1Char('/'))) {
        p += txt;
    } else {
        const int i = p.lastIndexOf(QLatin1Char('/'));
        if (i < 0)
            p = QLatin1Char('/') + txt;
        else
            p = p.left(i + 1) + txt;
    }
    QUrl::setPath(p);
}

QString KUrl::fileName(const DirectoryOptions &options) const
{
    QString p = QUrl::path();
    if (p.isEmpty())
        return QString();
    if (!(options & ObeyTrailingSlash)) {
        while (p.length() > 1 && p.endsWith(QLatin1Char('/')))
            p.chop(1);
    } else if (p.endsWith(QLatin1Char('/'))) {
        return QString();
    }
    const int i = p.lastIndexOf(QLatin1Char('/'));
    return i < 0 ? p : p.mid(i + 1);
}

QString KUrl::directory(const DirectoryOptions &options) const
{
    QString p = QUrl::path();
    if (p.isEmpty())
        return QString();
    if (!(options & ObeyTrailingSlash)) {
        while (p.length() > 1 && p.endsWith(QLatin1Char('/')))
            p.chop(1);
    }
    const int i = p.lastIndexOf(QLatin1Char('/'));
    QString dir;
    if (i < 0)
        dir = QString();
    else if (i == 0)
        dir = QLatin1String("/");
    else
        dir = (options & AppendTrailingSlash) ? p.left(i + 1) : p.left(i);
    return dir;
}

void KUrl::setDirectory(const QString &dir)
{
    if (dir.endsWith(QLatin1Char('/')))
        setPath(dir);
    else
        setPath(dir + QLatin1Char('/'));
}

bool KUrl::cd(const QString &dir)
{
    if (dir.isEmpty() || !isValid())
        return false;
    if (dir.startsWith(QLatin1Char('/'))) {
        setPath(dir);
        return true;
    }
    QString p = path(AddTrailingSlash);
    if (p.isEmpty())
        p = QLatin1String("/");
    p += dir;
    setPath(QDir::cleanPath(p));
    setFragment(QString());
    setEncodedQuery(QByteArray());
    return true;
}

QString KUrl::url(AdjustPathOption trailing) const
{
    if (trailing == LeaveTrailingSlash)
        return QString::fromLatin1(toEncoded());
    KUrl u(*this);
    u.adjustPath(trailing);
    return QString::fromLatin1(u.toEncoded());
}

QString KUrl::prettyUrl(AdjustPathOption trailing) const
{
    KUrl u(*this);
    u.adjustPath(trailing);
    u.setPassword(QString());
    if (u.isLocalFile())
        return QLatin1String("file://") + u.QUrl::path();
    return u.toString();
}

QString KUrl::pathOrUrl() const
{
    return pathOrUrl(LeaveTrailingSlash);
}

QString KUrl::pathOrUrl(AdjustPathOption trailing) const
{
    if (isLocalFile() && !hasFragment() && !hasQuery())
        return toLocalFile(trailing);
    return prettyUrl(trailing);
}

QString KUrl::toMimeDataString() const
{
    return url();
}

KUrl KUrl::upUrl() const
{
    KUrl u(*this);
    u.setFragment(QString());
    u.setEncodedQuery(QByteArray());
    QString p = QUrl::path();
    if (p.isEmpty() || p == QLatin1String("/"))
        return u;
    while (p.length() > 1 && p.endsWith(QLatin1Char('/')))
        p.chop(1);
    const int i = p.lastIndexOf(QLatin1Char('/'));
    if (i <= 0)
        p = QLatin1String("/");
    else
        p = p.left(i + 1);
    u.QUrl::setPath(p);
    return u;
}

KUrl::List KUrl::split(const QString &url)
{
    return List(KUrl(url));
}

KUrl::List KUrl::split(const KUrl &url)
{
    return List(url);
}

KUrl KUrl::join(const List &list)
{
    return list.isEmpty() ? KUrl() : list.first();
}

KUrl KUrl::fromPath(const QString &text)
{
    KUrl u;
    u.setPath(text);
    return u;
}

KUrl KUrl::fromMimeDataByteArray(const QByteArray &str)
{
    return KUrl(QUrl::fromEncoded(str, QUrl::TolerantMode));
}

bool KUrl::isRelativeUrl(const QString &url)
{
    const int colon = url.indexOf(QLatin1Char(':'));
    if (colon <= 0)
        return true;
    for (int i = 0; i < colon; ++i) {
        const QChar c = url.at(i);
        if (!c.isLetterOrNumber() && c != QLatin1Char('+') && c != QLatin1Char('-') && c != QLatin1Char('.'))
            return true;
    }
    return false;
}

QString KUrl::relativePath(const QString &base_dir, const QString &path, bool *isParent)
{
    const QString rel = QDir(base_dir).relativeFilePath(path);
    if (isParent)
        *isParent = !rel.startsWith(QLatin1String(".."));
    return rel;
}

QString KUrl::relativeUrl(const KUrl &base_url, const KUrl &url)
{
    if (base_url.scheme() != url.scheme() || base_url.host() != url.host())
        return url.url();
    return relativePath(base_url.directory(), url.path());
}

void KUrl::populateMimeData(QMimeData *mimeData, const MetaDataMap &metaData, MimeDataFlags flags) const
{
    List(*this).populateMimeData(mimeData, metaData, flags);
}

QDataStream &operator<<(QDataStream &s, const KUrl &a)
{
    s << a.url();
    return s;
}

QDataStream &operator>>(QDataStream &s, KUrl &a)
{
    QString str;
    s >> str;
    a = KUrl(str);
    return s;
}

uint qHash(const KUrl &kurl)
{
    return qHash(kurl.url());
}
