/* qt4compat: kmimetype.h - KMimeType without ksycoca, backed by a compiled-in
 * subset of the shared-mime-info database (see tools/gen-mimedata.py) and a
 * small content sniffer. */
#ifndef KDECOMPAT_KMIMETYPE_H
#define KDECOMPAT_KMIMETYPE_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <ksharedptr.h>
#include <kurl.h>

#include <sys/types.h>

class QIODevice;
class KMimeTypePrivate;
class KMimeTypeRepository;

class KDECORE_EXPORT KMimeType : public QSharedData
{
    friend class KMimeTypeRepository;
public:
    typedef KSharedPtr<KMimeType> Ptr;
    typedef QList<Ptr> List;

    enum FindByNameOption { DontResolveAlias = 0, ResolveAliases = 1 };

    virtual ~KMimeType();

    QString name() const;
    QString comment() const;
    QString comment(const KUrl &url) const;
    QStringList patterns() const;
    QString iconName(const KUrl &url = KUrl()) const;
    QString userSpecifiedIconName() const;
    QString mainExtension() const;
    bool is(const QString &mimeTypeName) const;
    QString parentMimeType() const;
    QStringList parentMimeTypes() const;
    QStringList allParentMimeTypes() const;
    bool isDefault() const;
    bool isValid() const;

    static Ptr mimeType(const QString &name, FindByNameOption options = ResolveAliases);
    static Ptr findByUrl(const KUrl &url, mode_t mode = 0, bool is_local_file = false, bool fast_mode = false, int *accuracy = 0);
    static Ptr findByPath(const QString &path, mode_t mode = 0, bool fast_mode = false, int *accuracy = 0);
    static Ptr findByContent(const QByteArray &data, int *accuracy = 0);
    static Ptr findByContent(QIODevice *device, int *accuracy = 0);
    static Ptr findByNameAndContent(const QString &name, const QByteArray &data, mode_t mode = 0, int *accuracy = 0);
    static Ptr findByNameAndContent(const QString &name, QIODevice *device, mode_t mode = 0, int *accuracy = 0);
    static Ptr findByFileContent(const QString &fileName, int *accuracy = 0);
    static List allMimeTypes();
    static QString defaultMimeType();
    static Ptr defaultMimeTypePtr();
    static bool isBinaryData(const QByteArray &data);
    static bool isBufferBinaryData(const QByteArray &data);
    static QString iconNameForUrl(const KUrl &url);
    static QString favIconForUrl(const KUrl &url);
    static QString extractKnownExtension(const QString &fileName);
    static bool matchFileName(const QString &filename, const QString &pattern);
    static int sharedMimeInfoVersion();

protected:
    KMimeType(const QString &name, const QString &comment, const QStringList &patterns,
              const QStringList &parents, const QString &icon);

private:
    KMimeTypePrivate *const d;
};

#endif
