/* qt4compat: kurl.h - KUrl on top of QUrl (Qt 4.8) */
#ifndef KDECOMPAT_KURL_H
#define KDECOMPAT_KURL_H

#include <QtCore/QDataStream>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <kdemacros.h>

class QMimeData;

class KDECORE_EXPORT KUrl : public QUrl
{
public:
    typedef QMap<QString, QString> MetaDataMap;

    enum MimeDataFlags { DefaultMimeDataFlags = 0, NoTextExport = 1 };

    enum AdjustPathOption {
        RemoveTrailingSlash,
        LeaveTrailingSlash,
        AddTrailingSlash
    };

    class KDECORE_EXPORT List : public QList<KUrl>
    {
    public:
        List() {}
        List(const KUrl &url);
        List(const QStringList &list);
        List(const QList<KUrl> &list);
        List(const QList<QUrl> &list);

        QStringList toStringList() const;
        QStringList toStringList(KUrl::AdjustPathOption trailing) const;

        void populateMimeData(QMimeData *mimeData, const KUrl::MetaDataMap &metaData = MetaDataMap(),
                              MimeDataFlags flags = DefaultMimeDataFlags) const;
        void populateMimeData(const KUrl::List &mostLocalUrls, QMimeData *mimeData,
                              const KUrl::MetaDataMap &metaData = MetaDataMap(),
                              MimeDataFlags flags = DefaultMimeDataFlags) const;
        static bool canDecode(const QMimeData *mimeData);
        static QStringList mimeDataTypes();
        enum DecodeOptions { PreferLocalUrls, PreferKdeUrls };
        static KUrl::List fromMimeData(const QMimeData *mimeData, KUrl::MetaDataMap *metaData = 0);
        static KUrl::List fromMimeData(const QMimeData *mimeData, DecodeOptions decodeOptions, KUrl::MetaDataMap *metaData = 0);
    };

    KUrl();
    KUrl(const QString &urlOrPath);
    explicit KUrl(const char *urlOrPath);
    explicit KUrl(const QByteArray &urlOrPath);
    KUrl(const KUrl &u);
    KUrl(const QUrl &u);
    KUrl(const KUrl &base, const QString &relative);
    ~KUrl();

    KUrl &operator=(const KUrl &u);
    KUrl &operator=(const QUrl &u);
    KUrl &operator=(const QString &s);
    operator QVariant() const;

    bool operator==(const KUrl &u) const;
    bool operator==(const QString &s) const;
    bool operator!=(const KUrl &u) const { return !(*this == u); }
    bool operator!=(const QString &s) const { return !(*this == s); }

    enum EqualsOption {
        CompareWithoutTrailingSlash = 0x01,
        CompareWithoutFragment = 0x02,
        AllowEmptyPath = 0x04
    };
    Q_DECLARE_FLAGS(EqualsOptions, EqualsOption)

    bool equals(const KUrl &u, const EqualsOptions &options = 0) const;
    bool isParentOf(const KUrl &u) const;

    QString protocol() const;
    void setProtocol(const QString &proto);
    QString user() const;
    void setUser(const QString &user);
    bool hasUser() const;
    QString pass() const;
    void setPass(const QString &pass);
    bool hasPass() const;
    bool hasHost() const;

    QString path(AdjustPathOption trailing = LeaveTrailingSlash) const;
    QString toLocalFile(AdjustPathOption trailing = LeaveTrailingSlash) const;
    void setPath(const QString &path);
    bool hasPath() const;

    enum CleanPathOption {
        SimplifyDirSeparators = 0x00,
        KeepDirSeparators = 0x01
    };
    Q_DECLARE_FLAGS(CleanPathOptions, CleanPathOption)
    void cleanPath(const CleanPathOptions &options = SimplifyDirSeparators);
    void adjustPath(AdjustPathOption trailing);

    void setEncodedPathAndQuery(const QString &txt);
    void setEncodedPath(const QString &txt);
    enum EncodedPathAndQueryOption { PermitEmptyPath = 0x00, AvoidEmptyPath = 0x01 };
    Q_DECLARE_FLAGS(EncodedPathAndQueryOptions, EncodedPathAndQueryOption)
    QString encodedPathAndQuery(AdjustPathOption trailing = LeaveTrailingSlash,
                                const EncodedPathAndQueryOptions &options = PermitEmptyPath) const;

    void setQuery(const QString &query);
    QString query() const;
    QString ref() const;
    void setRef(const QString &fragment);
    bool hasRef() const;
    QString htmlRef() const;
    QString encodedHtmlRef() const;
    void setHTMLRef(const QString &ref);
    bool hasHTMLRef() const;

    bool isLocalFile() const;
    void setFileEncoding(const QString &encoding);
    QString fileEncoding() const;
    bool hasSubUrl() const;

    void addPath(const QString &txt);
    QString queryItem(const QString &item) const;
    void addQueryItem(const QString &item, const QString &value);

    enum DirectoryOption {
        AppendTrailingSlash = 0x02,
        ObeyTrailingSlash = 0x04,
        IgnoreTrailingSlash = 0x01
    };
    Q_DECLARE_FLAGS(DirectoryOptions, DirectoryOption)

    void setFileName(const QString &txt);
    QString fileName(const DirectoryOptions &options = IgnoreTrailingSlash) const;
    QString directory(const DirectoryOptions &options = IgnoreTrailingSlash) const;
    void setDirectory(const QString &dir);
    bool cd(const QString &dir);

    QString url(AdjustPathOption trailing = LeaveTrailingSlash) const;
    QString prettyUrl(AdjustPathOption trailing = LeaveTrailingSlash) const;
    QString pathOrUrl() const;
    QString pathOrUrl(AdjustPathOption trailing) const;
    QString toMimeDataString() const;
    KUrl upUrl() const;

    static List split(const QString &url);
    static List split(const KUrl &url);
    static KUrl join(const List &list);
    static KUrl fromPath(const QString &text);
    static KUrl fromMimeDataByteArray(const QByteArray &str);
    static bool isRelativeUrl(const QString &url);
    static QString relativeUrl(const KUrl &base_url, const KUrl &url);
    static QString relativePath(const QString &base_dir, const QString &path, bool *isParent = 0);

    void populateMimeData(QMimeData *mimeData, const MetaDataMap &metaData = MetaDataMap(),
                          MimeDataFlags flags = DefaultMimeDataFlags) const;

private:
    void init(const QString &urlOrPath);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KUrl::EqualsOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KUrl::DirectoryOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KUrl::CleanPathOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KUrl::EncodedPathAndQueryOptions)

Q_DECLARE_METATYPE(KUrl)
Q_DECLARE_METATYPE(KUrl::List)

KDECORE_EXPORT QDataStream &operator<<(QDataStream &s, const KUrl &a);
KDECORE_EXPORT QDataStream &operator>>(QDataStream &s, KUrl &a);
KDECORE_EXPORT uint qHash(const KUrl &kurl);

#endif
