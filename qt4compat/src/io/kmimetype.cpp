/* qt4compat: kmimetype.cpp */
#include <kmimetype.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QIODevice>
#include <QtCore/QRegExp>
#include <QtCore/QSet>
#include <QtGui/QIcon>

#include <sys/stat.h>

#include "kmimetype_data_p.h"

class KMimeTypePrivate
{
public:
    QString name;
    QString comment;
    QStringList patterns;
    QStringList parents;
    QString icon;
};

/* ------------------------------------------------------------------------- */

namespace {

struct GlobRef
{
    int type;           /* index into repository list */
    int weight;
    bool caseSensitive;
    QString pattern;
};

struct Match
{
    int type;
    int weight;
    int length;
};

static bool matchBetter(const Match &a, const Match &b)
{
    if (a.weight != b.weight)
        return a.weight > b.weight;
    return a.length > b.length;
}

static bool hasWildcards(const QString &s)
{
    return s.contains(QLatin1Char('*')) || s.contains(QLatin1Char('?')) || s.contains(QLatin1Char('['));
}

} // namespace

class KMimeTypeRepository
{
public:
    KMimeTypeRepository();
    static KMimeTypeRepository *self();

    KMimeType::Ptr byName(const QString &name, bool resolveAlias) const;
    KMimeType::Ptr byFileName(const QString &fileName, int *accuracy, QString *matchedExtension = 0) const;
    KMimeType::Ptr byContent(const QByteArray &data, int *accuracy) const;
    KMimeType::Ptr typeOrDefault(const char *name) const;
    QString canonical(const QString &name) const;

    QList<KMimeType::Ptr> all;
    QHash<QString, int> index;              /* canonical name -> position in all */
    QHash<QString, QString> aliases;
    QHash<QString, QList<GlobRef> > literals;   /* lower-cased literal file names */
    QHash<QString, QList<GlobRef> > suffixes;   /* lower-cased "ext" of "*.ext" */
    QList<GlobRef> wildcards;
    KMimeType::Ptr defaultType;
};

KMimeTypeRepository *KMimeTypeRepository::self()
{
    static KMimeTypeRepository *s = 0;
    if (!s)
        s = new KMimeTypeRepository;
    return s;
}

KMimeTypeRepository::KMimeTypeRepository()
{
    for (int i = 0; i < kmime_aliases_count; ++i)
        aliases.insert(QString::fromLatin1(kmime_aliases[i].alias), QString::fromLatin1(kmime_aliases[i].canonical));

    for (int i = 0; i < kmime_entries_count; ++i) {
        const KMimeTypeEntry &e = kmime_entries[i];
        const QString name = QString::fromLatin1(e.name);
        QStringList patterns;
        const QStringList globs = QString::fromLatin1(e.globs).split(QLatin1Char('|'), QString::SkipEmptyParts);
        foreach (const QString &g, globs) {
            GlobRef ref;
            ref.type = i;
            QString spec = g;
            ref.caseSensitive = spec.startsWith(QLatin1Char('!'));
            if (ref.caseSensitive)
                spec = spec.mid(1);
            ref.weight = spec.left(2).toInt();
            ref.pattern = spec.mid(3);
            patterns << ref.pattern;
            if (!hasWildcards(ref.pattern)) {
                literals[ref.caseSensitive ? ref.pattern : ref.pattern.toLower()].append(ref);
            } else if (ref.pattern.startsWith(QLatin1String("*.")) && !hasWildcards(ref.pattern.mid(2))) {
                const QString ext = ref.pattern.mid(2);
                suffixes[ref.caseSensitive ? ext : ext.toLower()].append(ref);
            } else {
                wildcards.append(ref);
            }
        }
        const QStringList parents = QString::fromLatin1(e.parents).split(QLatin1Char('|'), QString::SkipEmptyParts);
        KMimeType *t = new KMimeType(name, QString::fromUtf8(e.comment), patterns, parents, QString::fromLatin1(e.icon));
        all.append(KMimeType::Ptr(t));
        index.insert(name, i);
    }
    defaultType = byName(QLatin1String("application/octet-stream"), false);
    if (!defaultType) {
        KMimeType *t = new KMimeType(QLatin1String("application/octet-stream"), QLatin1String("Unknown"), QStringList(), QStringList(), QString());
        defaultType = KMimeType::Ptr(t);
        index.insert(defaultType->name(), all.count());
        all.append(defaultType);
    }
}

QString KMimeTypeRepository::canonical(const QString &name) const
{
    QHash<QString, QString>::const_iterator it = aliases.constFind(name);
    return it == aliases.constEnd() ? name : it.value();
}

KMimeType::Ptr KMimeTypeRepository::byName(const QString &name, bool resolveAlias) const
{
    QString n = name.toLower();
    if (resolveAlias)
        n = canonical(n);
    QHash<QString, int>::const_iterator it = index.constFind(n);
    if (it == index.constEnd())
        return KMimeType::Ptr();
    return all.at(it.value());
}

KMimeType::Ptr KMimeTypeRepository::typeOrDefault(const char *name) const
{
    KMimeType::Ptr p = byName(QString::fromLatin1(name), true);
    return p ? p : defaultType;
}

KMimeType::Ptr KMimeTypeRepository::byFileName(const QString &fileName, int *accuracy, QString *matchedExtension) const
{
    QList<Match> matches;
    const QString lower = fileName.toLower();

    /* literal file names */
    QHash<QString, QList<GlobRef> >::const_iterator lit = literals.constFind(lower);
    if (lit != literals.constEnd()) {
        foreach (const GlobRef &ref, lit.value()) {
            if (!ref.caseSensitive) {
                Match m = { ref.type, ref.weight, ref.pattern.length() };
                matches << m;
            }
        }
    }
    lit = literals.constFind(fileName);
    if (lit != literals.constEnd()) {
        foreach (const GlobRef &ref, lit.value()) {
            if (ref.caseSensitive) {
                Match m = { ref.type, ref.weight, ref.pattern.length() };
                matches << m;
            }
        }
    }

    /* suffixes, longest first */
    QString bestExt;
    for (int pos = lower.indexOf(QLatin1Char('.')); pos >= 0; pos = lower.indexOf(QLatin1Char('.'), pos + 1)) {
        const QString extLower = lower.mid(pos + 1);
        const QString extExact = fileName.mid(pos + 1);
        if (extLower.isEmpty())
            break;
        QHash<QString, QList<GlobRef> >::const_iterator sit = suffixes.constFind(extLower);
        if (sit != suffixes.constEnd()) {
            foreach (const GlobRef &ref, sit.value()) {
                if (ref.caseSensitive && ref.pattern.mid(2) != extExact)
                    continue;
                Match m = { ref.type, ref.weight, ref.pattern.length() };
                matches << m;
                if (bestExt.isEmpty())
                    bestExt = extExact;
            }
        }
        if (bestExt.isEmpty()) {
            sit = suffixes.constFind(extExact);
            if (sit != suffixes.constEnd()) {
                foreach (const GlobRef &ref, sit.value()) {
                    if (!ref.caseSensitive)
                        continue;
                    Match m = { ref.type, ref.weight, ref.pattern.length() };
                    matches << m;
                    if (bestExt.isEmpty())
                        bestExt = extExact;
                }
            }
        }
    }

    /* generic wildcards */
    foreach (const GlobRef &ref, wildcards) {
        QRegExp rx(ref.pattern, ref.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, QRegExp::Wildcard);
        if (rx.exactMatch(fileName)) {
            Match m = { ref.type, ref.weight, ref.pattern.length() };
            matches << m;
        }
    }

    if (matches.isEmpty()) {
        if (accuracy)
            *accuracy = 0;
        return KMimeType::Ptr();
    }
    qStableSort(matches.begin(), matches.end(), matchBetter);
    if (accuracy)
        *accuracy = 80;
    if (matchedExtension)
        *matchedExtension = bestExt;
    return all.at(matches.first().type);
}

static bool startsWith(const QByteArray &data, const char *magic, int len, int offset = 0)
{
    if (data.size() < offset + len)
        return false;
    return memcmp(data.constData() + offset, magic, len) == 0;
}

static quint32 be32(const QByteArray &data, int offset)
{
    if (data.size() < offset + 4)
        return 0;
    const unsigned char *p = reinterpret_cast<const unsigned char *>(data.constData()) + offset;
    return (quint32(p[0]) << 24) | (quint32(p[1]) << 16) | (quint32(p[2]) << 8) | quint32(p[3]);
}

static const char *interpreterMimeType(const QString &interp)
{
    if (interp == QLatin1String("sh") || interp == QLatin1String("bash") || interp == QLatin1String("zsh")
        || interp == QLatin1String("ksh") || interp == QLatin1String("dash") || interp == QLatin1String("ash")
        || interp == QLatin1String("fish"))
        return "application/x-shellscript";
    if (interp == QLatin1String("csh") || interp == QLatin1String("tcsh"))
        return "application/x-csh";
    if (interp.startsWith(QLatin1String("python")))
        return "text/x-python";
    if (interp.startsWith(QLatin1String("perl")))
        return "application/x-perl";
    if (interp.startsWith(QLatin1String("ruby")))
        return "application/x-ruby";
    if (interp.startsWith(QLatin1String("php")))
        return "application/x-php";
    if (interp == QLatin1String("node") || interp == QLatin1String("nodejs") || interp == QLatin1String("js"))
        return "application/javascript";
    if (interp == QLatin1String("awk") || interp == QLatin1String("gawk") || interp == QLatin1String("mawk") || interp == QLatin1String("nawk"))
        return "application/x-awk";
    if (interp.startsWith(QLatin1String("lua")))
        return "text/x-lua";
    if (interp == QLatin1String("tclsh") || interp == QLatin1String("wish") || interp == QLatin1String("tcl") || interp == QLatin1String("expect"))
        return "text/x-tcl";
    if (interp == QLatin1String("make") || interp == QLatin1String("gmake"))
        return "text/x-makefile";
    if (interp == QLatin1String("osascript"))
        return "text/x-applescript";
    return 0;
}

KMimeType::Ptr KMimeTypeRepository::byContent(const QByteArray &data, int *accuracy) const
{
    int acc = 80;
    const char *type = 0;

    if (data.isEmpty()) {
        if (accuracy)
            *accuracy = 0;
        return typeOrDefault("text/plain");
    }

    if (startsWith(data, "\x7f" "ELF", 4)) {
        if (data.size() > 17) {
            const unsigned char cls = data.at(4);
            const unsigned char *p = reinterpret_cast<const unsigned char *>(data.constData());
            const bool little = data.at(5) == 1;
            const unsigned short etype = little ? (p[16] | (p[17] << 8)) : ((p[16] << 8) | p[17]);
            Q_UNUSED(cls)
            if (etype == 1) type = "application/x-object";
            else if (etype == 3) type = "application/x-sharedlib";
            else if (etype == 4) type = "application/x-core";
            else type = "application/x-executable";
        } else {
            type = "application/x-executable";
        }
    } else if (startsWith(data, "\xfe\xed\xfa\xce", 4) || startsWith(data, "\xfe\xed\xfa\xcf", 4)
               || startsWith(data, "\xce\xfa\xed\xfe", 4) || startsWith(data, "\xcf\xfa\xed\xfe", 4)) {
        type = "application/x-mach-binary";
    } else if (startsWith(data, "\xca\xfe\xba\xbe", 4)) {
        /* fat Mach-O has a small architecture count, Java class files a version >= 45 */
        type = be32(data, 4) < 64 ? "application/x-mach-binary" : "application/x-java";
    } else if (startsWith(data, "%PDF", 4)) {
        type = "application/pdf";
    } else if (startsWith(data, "%!PS", 4) || startsWith(data, "\x04%!PS", 5)) {
        type = "application/postscript";
    } else if (startsWith(data, "\x1f\x8b", 2)) {
        type = "application/gzip";
    } else if (startsWith(data, "BZh", 3)) {
        type = "application/x-bzip2";
    } else if (startsWith(data, "\xfd" "7zXZ", 5)) {
        type = "application/x-xz";
    } else if (startsWith(data, "7z\xbc\xaf\x27\x1c", 6)) {
        type = "application/x-7z-compressed";
    } else if (startsWith(data, "Rar!", 4)) {
        type = "application/vnd.rar";
    } else if (startsWith(data, "\x28\xb5\x2f\xfd", 4)) {
        type = "application/zstd";
    } else if (startsWith(data, "PK\x03\x04", 4) || startsWith(data, "PK\x05\x06", 4)) {
        type = "application/zip";
    } else if (startsWith(data, "!<arch>", 7)) {
        type = "application/x-archive";
    } else if (startsWith(data, "\xed\xab\xee\xdb", 4)) {
        type = "application/x-rpm";
    } else if (startsWith(data, "ustar", 5, 257)) {
        type = "application/x-tar";
    } else if (startsWith(data, "SQLite format 3", 15)) {
        type = "application/vnd.sqlite3";
    } else if (startsWith(data, "\x89PNG\r\n\x1a\n", 8)) {
        type = "image/png";
    } else if (startsWith(data, "\xff\xd8\xff", 3)) {
        type = "image/jpeg";
    } else if (startsWith(data, "GIF87a", 6) || startsWith(data, "GIF89a", 6)) {
        type = "image/gif";
    } else if (startsWith(data, "BM", 2) && data.size() > 14 && data.at(6) == 0 && data.at(7) == 0) {
        type = "image/bmp";
    } else if (startsWith(data, "\x00\x00\x01\x00", 4)) {
        type = "image/vnd.microsoft.icon";
    } else if (startsWith(data, "icns", 4)) {
        type = "image/x-icns";
    } else if (startsWith(data, "II*\x00", 4) || startsWith(data, "MM\x00*", 4)) {
        type = "image/tiff";
    } else if (startsWith(data, "RIFF", 4) && startsWith(data, "WAVE", 4, 8)) {
        type = "audio/x-wav";
    } else if (startsWith(data, "RIFF", 4) && startsWith(data, "AVI ", 4, 8)) {
        type = "video/x-msvideo";
    } else if (startsWith(data, "RIFF", 4) && startsWith(data, "WEBP", 4, 8)) {
        type = "image/webp";
    } else if (startsWith(data, "OggS", 4)) {
        type = "application/ogg";
    } else if (startsWith(data, "fLaC", 4)) {
        type = "audio/flac";
    } else if (startsWith(data, "ID3", 3) || startsWith(data, "\xff\xfb", 2)) {
        type = "audio/mpeg";
    } else if (startsWith(data, "\x1a\x45\xdf\xa3", 4)) {
        type = "video/x-matroska";
    } else if (startsWith(data, "ftyp", 4, 4)) {
        type = "video/mp4";
    } else if (startsWith(data, "MZ", 2)) {
        type = "application/x-ms-dos-executable";
    } else if (startsWith(data, "{\\rtf", 5)) {
        type = "application/rtf";
    } else if (startsWith(data, "%PDF", 4)) {
        type = "application/pdf";
    } else if (startsWith(data, "\xef\xbb\xbf<?xml", 8) || startsWith(data, "<?xml", 5)) {
        acc = 70;
        type = "application/xml";
    } else if (startsWith(data, "#!", 2)) {
        const int eol = data.indexOf('\n');
        QString line = QString::fromLatin1(data.constData() + 2, (eol < 0 ? data.size() : eol) - 2).trimmed();
        QStringList words = line.split(QRegExp(QLatin1String("\\s+")), QString::SkipEmptyParts);
        QString interp;
        if (!words.isEmpty()) {
            interp = words.first().section(QLatin1Char('/'), -1);
            if (interp == QLatin1String("env") && words.count() > 1) {
                int i = 1;
                while (i < words.count() && words.at(i).startsWith(QLatin1Char('-')))
                    ++i;
                if (i < words.count())
                    interp = words.at(i).section(QLatin1Char('/'), -1);
            }
        }
        type = interpreterMimeType(interp);
        if (!type) {
            type = "text/plain";
            acc = 50;
        }
    } else if (startsWith(data, "%YAML", 5)) {
        type = "application/x-yaml";
        acc = 60;
    }

    if (!type) {
        QByteArray head = data.left(512);
        const QByteArray lower = head.toLower();
        if (lower.contains("<!doctype html") || lower.contains("<html")) {
            type = "text/html";
            acc = 60;
        } else if (KMimeType::isBufferBinaryData(data)) {
            type = "application/octet-stream";
            acc = 20;
        } else {
            type = "text/plain";
            acc = 50;
        }
    }
    if (accuracy)
        *accuracy = acc;
    return typeOrDefault(type);
}

/* ------------------------------------------------------------------------- */

KMimeType::KMimeType(const QString &name, const QString &comment, const QStringList &patterns,
                     const QStringList &parents, const QString &icon)
    : QSharedData(), d(new KMimeTypePrivate)
{
    d->name = name;
    d->comment = comment;
    d->patterns = patterns;
    d->parents = parents;
    d->icon = icon;
}

KMimeType::~KMimeType()
{
    delete d;
}

QString KMimeType::name() const
{
    return d->name;
}

QString KMimeType::comment() const
{
    return d->comment.isEmpty() ? d->name : d->comment;
}

QString KMimeType::comment(const KUrl &) const
{
    return comment();
}

QStringList KMimeType::patterns() const
{
    return d->patterns;
}

QString KMimeType::mainExtension() const
{
    foreach (const QString &p, d->patterns) {
        if (p.startsWith(QLatin1String("*.")) && !hasWildcards(p.mid(2)))
            return p.mid(1);
    }
    return QString();
}

QString KMimeType::iconName(const KUrl &) const
{
    QString specific = d->name;
    specific.replace(QLatin1Char('/'), QLatin1Char('-'));
    if (QIcon::hasThemeIcon(specific))
        return specific;
    if (!d->icon.isEmpty() && QIcon::hasThemeIcon(d->icon))
        return d->icon;
    const QString major = d->name.section(QLatin1Char('/'), 0, 0);
    QString generic;
    if (major == QLatin1String("text"))
        generic = QLatin1String("text-x-generic");
    else if (major == QLatin1String("image"))
        generic = QLatin1String("image-x-generic");
    else if (major == QLatin1String("audio"))
        generic = QLatin1String("audio-x-generic");
    else if (major == QLatin1String("video"))
        generic = QLatin1String("video-x-generic");
    else if (major == QLatin1String("inode"))
        generic = d->name == QLatin1String("inode/directory") ? QLatin1String("folder") : QLatin1String("unknown");
    else if (!d->icon.isEmpty())
        generic = d->icon;
    else if (is(QLatin1String("text/plain")))
        generic = QLatin1String("text-x-generic");
    else
        generic = QLatin1String("unknown");
    if (!QIcon::hasThemeIcon(generic) && !d->icon.isEmpty())
        return d->icon;
    return generic;
}

QString KMimeType::userSpecifiedIconName() const
{
    return d->icon;
}

bool KMimeType::is(const QString &mimeTypeName) const
{
    const KMimeTypeRepository *repo = KMimeTypeRepository::self();
    const QString other = repo->canonical(mimeTypeName.toLower());
    if (d->name == other)
        return true;
    if (other == QLatin1String("application/octet-stream"))
        return true;
    if (other == QLatin1String("text/plain") && d->name.startsWith(QLatin1String("text/")))
        return true;
    QSet<QString> seen;
    QStringList todo = d->parents;
    while (!todo.isEmpty()) {
        const QString p = repo->canonical(todo.takeFirst());
        if (p == other)
            return true;
        if (seen.contains(p))
            continue;
        seen.insert(p);
        if (other == QLatin1String("text/plain") && p.startsWith(QLatin1String("text/")))
            return true;
        const Ptr pp = repo->byName(p, true);
        if (pp)
            todo << pp->d->parents;
    }
    return false;
}

QString KMimeType::parentMimeType() const
{
    return d->parents.isEmpty() ? QString() : KMimeTypeRepository::self()->canonical(d->parents.first());
}

QStringList KMimeType::parentMimeTypes() const
{
    QStringList result;
    foreach (const QString &p, d->parents)
        result << KMimeTypeRepository::self()->canonical(p);
    if (result.isEmpty() && d->name.startsWith(QLatin1String("text/")) && d->name != QLatin1String("text/plain"))
        result << QLatin1String("text/plain");
    return result;
}

QStringList KMimeType::allParentMimeTypes() const
{
    const KMimeTypeRepository *repo = KMimeTypeRepository::self();
    QStringList result;
    QStringList todo = parentMimeTypes();
    while (!todo.isEmpty()) {
        const QString p = todo.takeFirst();
        if (result.contains(p))
            continue;
        result << p;
        const Ptr pp = repo->byName(p, true);
        if (pp)
            todo << pp->parentMimeTypes();
    }
    return result;
}

bool KMimeType::isDefault() const
{
    return d->name == QLatin1String("application/octet-stream");
}

bool KMimeType::isValid() const
{
    return !d->name.isEmpty();
}

KMimeType::Ptr KMimeType::mimeType(const QString &name, FindByNameOption options)
{
    return KMimeTypeRepository::self()->byName(name, options == ResolveAliases);
}

KMimeType::Ptr KMimeType::findByUrl(const KUrl &url, mode_t mode, bool is_local_file, bool fast_mode, int *accuracy)
{
    if (is_local_file || url.isLocalFile())
        return findByPath(url.toLocalFile(), mode, fast_mode, accuracy);
    KMimeTypeRepository *repo = KMimeTypeRepository::self();
    if (S_ISDIR(mode) || url.path().endsWith(QLatin1Char('/'))) {
        if (accuracy)
            *accuracy = 100;
        return repo->typeOrDefault("inode/directory");
    }
    Ptr p = repo->byFileName(url.fileName(), accuracy);
    if (p)
        return p;
    if (accuracy)
        *accuracy = 0;
    return repo->defaultType;
}

KMimeType::Ptr KMimeType::findByPath(const QString &path, mode_t mode, bool fast_mode, int *accuracy)
{
    KMimeTypeRepository *repo = KMimeTypeRepository::self();
    QFileInfo fi(path);
    if (mode == 0 && fi.exists()) {
        if (fi.isDir())
            mode = S_IFDIR;
        else if (fi.isFile())
            mode = S_IFREG | (fi.isExecutable() ? S_IXUSR : 0);
    }
    if (S_ISDIR(mode)) {
        if (accuracy)
            *accuracy = 100;
        return repo->typeOrDefault("inode/directory");
    }
    int globAcc = 0;
    Ptr byGlob = repo->byFileName(fi.fileName(), &globAcc);
    if (fast_mode || !fi.isFile() || !fi.isReadable()) {
        if (byGlob) {
            if (accuracy)
                *accuracy = globAcc;
            return byGlob;
        }
        if (accuracy)
            *accuracy = 0;
        if ((mode & S_IXUSR) && fi.isFile())
            return repo->typeOrDefault("application/x-executable");
        return repo->defaultType;
    }
    QFile f(path);
    QByteArray head;
    if (f.open(QIODevice::ReadOnly))
        head = f.read(16384);
    int contentAcc = 0;
    Ptr byContent = head.isEmpty() ? Ptr() : repo->byContent(head, &contentAcc);
    if (byGlob) {
        if (byContent && contentAcc >= 80 && !byContent->is(QLatin1String("text/plain"))
            && !byGlob->is(byContent->name()) && !byContent->is(byGlob->name())) {
            /* the name lies: a compressed or binary file with a text extension */
            if (accuracy)
                *accuracy = contentAcc;
            return byContent;
        }
        if (accuracy)
            *accuracy = globAcc;
        return byGlob;
    }
    if (byContent) {
        if (accuracy)
            *accuracy = contentAcc;
        if (byContent->name() == QLatin1String("text/plain") && (mode & S_IXUSR) && !head.startsWith("#!")) {
            /* executable without a shebang and no extension: treat as script anyway */
            return byContent;
        }
        return byContent;
    }
    if (accuracy)
        *accuracy = 0;
    return head.isEmpty() ? repo->typeOrDefault("text/plain") : repo->defaultType;
}

KMimeType::Ptr KMimeType::findByContent(const QByteArray &data, int *accuracy)
{
    return KMimeTypeRepository::self()->byContent(data, accuracy);
}

KMimeType::Ptr KMimeType::findByContent(QIODevice *device, int *accuracy)
{
    if (!device) {
        if (accuracy)
            *accuracy = 0;
        return defaultMimeTypePtr();
    }
    const bool wasOpen = device->isOpen();
    if (!wasOpen && !device->open(QIODevice::ReadOnly)) {
        if (accuracy)
            *accuracy = 0;
        return defaultMimeTypePtr();
    }
    QByteArray head;
    if (device->isSequential()) {
        head = device->peek(16384);
    } else {
        const qint64 pos = device->pos();
        device->seek(0);
        head = device->read(16384);
        device->seek(pos);
    }
    if (!wasOpen)
        device->close();
    return findByContent(head, accuracy);
}

KMimeType::Ptr KMimeType::findByNameAndContent(const QString &name, const QByteArray &data, mode_t mode, int *accuracy)
{
    KMimeTypeRepository *repo = KMimeTypeRepository::self();
    if (S_ISDIR(mode)) {
        if (accuracy)
            *accuracy = 100;
        return repo->typeOrDefault("inode/directory");
    }
    int globAcc = 0;
    Ptr byGlob = repo->byFileName(QFileInfo(name).fileName(), &globAcc);
    int contentAcc = 0;
    Ptr byContent = data.isEmpty() ? Ptr() : repo->byContent(data, &contentAcc);
    if (byGlob && !(byContent && contentAcc >= 80 && !byContent->is(QLatin1String("text/plain"))
                    && !byGlob->is(byContent->name()) && !byContent->is(byGlob->name()))) {
        if (accuracy)
            *accuracy = globAcc;
        return byGlob;
    }
    if (byContent) {
        if (accuracy)
            *accuracy = contentAcc;
        return byContent;
    }
    if (accuracy)
        *accuracy = 0;
    return repo->defaultType;
}

KMimeType::Ptr KMimeType::findByNameAndContent(const QString &name, QIODevice *device, mode_t mode, int *accuracy)
{
    QByteArray head;
    if (device && (device->isOpen() || device->open(QIODevice::ReadOnly))) {
        if (device->isSequential()) {
            head = device->peek(16384);
        } else {
            const qint64 pos = device->pos();
            device->seek(0);
            head = device->read(16384);
            device->seek(pos);
        }
    }
    return findByNameAndContent(name, head, mode, accuracy);
}

KMimeType::Ptr KMimeType::findByFileContent(const QString &fileName, int *accuracy)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        if (accuracy)
            *accuracy = 0;
        return defaultMimeTypePtr();
    }
    return findByContent(f.read(16384), accuracy);
}

KMimeType::List KMimeType::allMimeTypes()
{
    return KMimeTypeRepository::self()->all;
}

QString KMimeType::defaultMimeType()
{
    return QLatin1String("application/octet-stream");
}

KMimeType::Ptr KMimeType::defaultMimeTypePtr()
{
    return KMimeTypeRepository::self()->defaultType;
}

bool KMimeType::isBinaryData(const QByteArray &data)
{
    return isBufferBinaryData(data);
}

bool KMimeType::isBinaryData(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    return isBufferBinaryData(file.read(512));
}

bool KMimeType::isBufferBinaryData(const QByteArray &data)
{
    const int n = qMin(data.size(), 512);
    const unsigned char *p = reinterpret_cast<const unsigned char *>(data.constData());
    for (int i = 0; i < n; ++i) {
        const unsigned char c = p[i];
        if (c == 0)
            return true;
        if (c < 32 && c != '\n' && c != '\r' && c != '\t' && c != '\f' && c != '\v' && c != '\b' && c != 27)
            return true;
    }
    return false;
}

QString KMimeType::iconNameForUrl(const KUrl &url)
{
    Ptr p = findByUrl(url, 0, url.isLocalFile(), true);
    return p ? p->iconName(url) : QLatin1String("unknown");
}

QString KMimeType::favIconForUrl(const KUrl &)
{
    return QString();
}

QString KMimeType::extractKnownExtension(const QString &fileName)
{
    QString ext;
    KMimeTypeRepository::self()->byFileName(QFileInfo(fileName).fileName(), 0, &ext);
    return ext;
}

bool KMimeType::matchFileName(const QString &filename, const QString &pattern)
{
    QRegExp rx(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
    return rx.exactMatch(filename);
}

int KMimeType::sharedMimeInfoVersion()
{
    return 0;
}
