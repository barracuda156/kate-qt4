/* qt4compat: kstandarddirs.cpp */
#include <kstandarddirs.h>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <kde_file.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QSet>

#ifndef KATE_INSTALL_DATADIR
#define KATE_INSTALL_DATADIR ""
#endif
#ifndef KATE_SOURCE_DATADIRS
#define KATE_SOURCE_DATADIRS ""
#endif

class KStandardDirsPrivate
{
public:
    QString componentName;
    QStringList prefixes;
    QHash<QByteArray, QStringList> extraDirs;
    QHash<QByteArray, QPair<QByteArray, QString> > extraTypes;
};

static QString withSlash(const QString &s)
{
    if (s.isEmpty() || s.endsWith(QLatin1Char('/')))
        return s;
    return s + QLatin1Char('/');
}

static QString appDir()
{
    if (QCoreApplication::instance())
        return QCoreApplication::applicationDirPath();
    return QString();
}

QStringList KStandardDirs::systemDataDirs()
{
    static QStringList dirs;
    static bool haveApp = false;
    if (!dirs.isEmpty() && (haveApp || !QCoreApplication::instance()))
        return dirs;

    dirs.clear();
    haveApp = QCoreApplication::instance() != 0;

    const QByteArray env = qgetenv("KATE_RESOURCE_DIRS");
    if (!env.isEmpty()) {
        foreach (const QString &d, QString::fromLocal8Bit(env).split(QLatin1Char(':'), QString::SkipEmptyParts))
            dirs << QDir::cleanPath(d);
    }
    const QString app = appDir();
    if (!app.isEmpty()) {
#ifdef Q_OS_MAC
        const QString res = QDir::cleanPath(app + QLatin1String("/../Resources"));
        if (QDir(res).exists())
            dirs << res;
#endif
        const QString share = QDir::cleanPath(app + QLatin1String("/../share/kate-qt4"));
        if (QDir(share).exists())
            dirs << share;
        const QString devShare = QDir::cleanPath(app + QLatin1String("/share"));
        if (QDir(devShare).exists())
            dirs << devShare;
    }
    const QString installed = QString::fromLatin1(KATE_INSTALL_DATADIR);
    if (!installed.isEmpty() && QDir(installed).exists())
        dirs << installed;
    const QString sourceDirs = QString::fromLatin1(KATE_SOURCE_DATADIRS);
    if (!sourceDirs.isEmpty()) {
        foreach (const QString &d, sourceDirs.split(QLatin1Char(':'), QString::SkipEmptyParts))
            if (QDir(d).exists())
                dirs << QDir::cleanPath(d);
    }
    dirs.removeDuplicates();
    return dirs;
}

QString KStandardDirs::userDataDir()
{
    static QString dir;
    if (dir.isEmpty()) {
#ifdef Q_OS_MAC
        dir = QDir::homePath() + QLatin1String("/Library/Application Support/Kate");
#else
        QString base = QString::fromLocal8Bit(qgetenv("XDG_DATA_HOME"));
        if (base.isEmpty())
            base = QDir::homePath() + QLatin1String("/.local/share");
        dir = base + QLatin1String("/kate-qt4");
#endif
    }
    return dir;
}

QString KStandardDirs::userConfigDir()
{
    static QString dir;
    if (dir.isEmpty()) {
#ifdef Q_OS_MAC
        dir = QDir::homePath() + QLatin1String("/Library/Preferences/Kate");
#else
        QString base = QString::fromLocal8Bit(qgetenv("XDG_CONFIG_HOME"));
        if (base.isEmpty())
            base = QDir::homePath() + QLatin1String("/.config");
        dir = base + QLatin1String("/kate-qt4");
#endif
    }
    return dir;
}

KStandardDirs::KStandardDirs()
    : d(new KStandardDirsPrivate)
{
}

KStandardDirs::~KStandardDirs()
{
    delete d;
}

void KStandardDirs::setComponentName(const QString &name)
{
    d->componentName = name;
}

void KStandardDirs::addPrefix(const QString &dir)
{
    if (!dir.isEmpty() && !d->prefixes.contains(dir))
        d->prefixes << QDir::cleanPath(dir);
}

void KStandardDirs::addXdgConfigPrefix(const QString &dir)
{
    addResourceDir("config", dir, true);
}

void KStandardDirs::addXdgDataPrefix(const QString &dir)
{
    addResourceDir("data", dir, true);
}

bool KStandardDirs::addResourceType(const char *type, const char *basetype, const QString &relativename, bool)
{
    if (!type || !basetype)
        return false;
    d->extraTypes.insert(QByteArray(type), qMakePair(QByteArray(basetype), withSlash(relativename)));
    return true;
}

bool KStandardDirs::addResourceDir(const char *type, const QString &absdir, bool priority)
{
    if (!type || absdir.isEmpty())
        return false;
    QStringList &list = d->extraDirs[QByteArray(type)];
    const QString dir = withSlash(QDir::cleanPath(absdir));
    if (list.contains(dir))
        return false;
    if (priority)
        list.prepend(dir);
    else
        list.append(dir);
    return true;
}

static QString userDirFor(const KStandardDirsPrivate *d, const QByteArray &type);

static QStringList systemDirsFor(const KStandardDirsPrivate *d, const QByteArray &type)
{
    QStringList bases = KStandardDirs::systemDataDirs();
    foreach (const QString &p, d->prefixes) {
        bases << p;
        bases << p + QLatin1String("/share/kate-qt4");
    }
    QStringList result;
    if (d->extraTypes.contains(type)) {
        const QPair<QByteArray, QString> spec = d->extraTypes.value(type);
        foreach (const QString &b, systemDirsFor(d, spec.first))
            result << b + spec.second;
        return result;
    }
    if (type == "data") {
        foreach (const QString &b, bases)
            result << withSlash(b);
    } else if (type == "appdata") {
        foreach (const QString &b, bases)
            result << withSlash(b) + withSlash(d->componentName);
    } else if (type == "config") {
        foreach (const QString &b, bases)
            result << withSlash(b) + QLatin1String("config/");
    } else if (type == "tmp") {
        result << withSlash(QDir::tempPath());
    } else if (type == "cache") {
        /* user only */
    } else if (type == "lib" || type == "exe" || type == "module" || type == "libexec" || type == "kdedir") {
        const QString app = appDir();
        if (!app.isEmpty())
            result << withSlash(app);
    } else if (type == "home") {
        result << withSlash(QDir::homePath());
    } else {
        foreach (const QString &b, bases)
            result << withSlash(b) + QString::fromLatin1(type.constData()) + QLatin1Char('/');
    }
    return result;
}

static QString userDirFor(const KStandardDirsPrivate *d, const QByteArray &type)
{
    if (d->extraTypes.contains(type)) {
        const QPair<QByteArray, QString> spec = d->extraTypes.value(type);
        const QString base = userDirFor(d, spec.first);
        return base.isEmpty() ? base : base + spec.second;
    }
    if (type == "data")
        return withSlash(KStandardDirs::userDataDir());
    if (type == "appdata")
        return withSlash(KStandardDirs::userDataDir()) + withSlash(d->componentName);
    if (type == "config")
        return withSlash(KStandardDirs::userConfigDir());
    if (type == "tmp")
        return withSlash(QDir::tempPath());
    if (type == "cache")
        return withSlash(KStandardDirs::userDataDir()) + QLatin1String("cache/");
    if (type == "home")
        return withSlash(QDir::homePath());
    if (type == "lib" || type == "exe" || type == "module" || type == "libexec" || type == "kdedir")
        return QString();
    return withSlash(KStandardDirs::userDataDir()) + QString::fromLatin1(type.constData()) + QLatin1Char('/');
}

QStringList KStandardDirs::resourceDirs(const char *type) const
{
    const QByteArray t(type);
    QStringList dirs;
    const QString user = userDirFor(d, t);
    if (!user.isEmpty())
        dirs << user;
    dirs << d->extraDirs.value(t);
    dirs << systemDirsFor(d, t);
    dirs.removeDuplicates();
    return dirs;
}

QStringList KStandardDirs::allTypes() const
{
    QStringList types;
    types << QLatin1String("data") << QLatin1String("appdata") << QLatin1String("config")
          << QLatin1String("tmp") << QLatin1String("cache") << QLatin1String("icon") << QLatin1String("locale");
    foreach (const QByteArray &t, d->extraTypes.keys())
        types << QString::fromLatin1(t.constData());
    return types;
}

QString KStandardDirs::localkdedir() const
{
    return withSlash(userDataDir());
}

QString KStandardDirs::localxdgdatadir() const
{
    return withSlash(userDataDir());
}

QString KStandardDirs::localxdgconfdir() const
{
    return withSlash(userConfigDir());
}

QString KStandardDirs::findResource(const char *type, const QString &filename) const
{
    if (QDir::isAbsolutePath(filename))
        return QFile::exists(filename) ? filename : QString();
    foreach (const QString &dir, resourceDirs(type)) {
        const QString full = dir + filename;
        if (QFile::exists(full))
            return full;
    }
    return QString();
}

QString KStandardDirs::findResourceDir(const char *type, const QString &filename) const
{
    foreach (const QString &dir, resourceDirs(type)) {
        if (QFile::exists(dir + filename))
            return dir;
    }
    return QString();
}

static void lookupFiles(const QString &base, const QString &relPrefix, const QString &pattern, bool recursive,
                        bool noDuplicates, QSet<QString> &seen, QStringList &result, QStringList &relPaths,
                        const QString &dirPart)
{
    QDir dir(base + relPrefix);
    if (!dir.exists())
        return;
    const QStringList files = dir.entryList(QStringList(pattern), QDir::Files | QDir::Readable, QDir::Name);
    foreach (const QString &f, files) {
        const QString rel = relPrefix + f;
        if (noDuplicates) {
            if (seen.contains(rel))
                continue;
            seen.insert(rel);
        }
        result << base + rel;
        relPaths << dirPart + rel;
    }
    if (recursive) {
        const QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        foreach (const QString &s, subdirs)
            lookupFiles(base, relPrefix + s + QLatin1Char('/'), pattern, recursive, noDuplicates, seen, result, relPaths, dirPart);
    }
}

QStringList KStandardDirs::findAllResources(const char *type, const QString &filter, SearchOptions options,
                                            QStringList &relPaths) const
{
    QString dirPart;
    QString filePart = filter;
    const int slash = filter.lastIndexOf(QLatin1Char('/'));
    if (slash >= 0) {
        dirPart = filter.left(slash + 1);
        filePart = filter.mid(slash + 1);
    }
    if (filePart.isEmpty())
        filePart = QLatin1String("*");

    QStringList result;
    QSet<QString> seen;
    if (QDir::isAbsolutePath(dirPart)) {
        lookupFiles(dirPart, QString(), filePart, options & Recursive, options & NoDuplicates, seen, result, relPaths, QString());
        return result;
    }
    foreach (const QString &dir, resourceDirs(type))
        lookupFiles(dir + dirPart, QString(), filePart, options & Recursive, options & NoDuplicates, seen, result, relPaths, dirPart);
    return result;
}

QStringList KStandardDirs::findAllResources(const char *type, const QString &filter, SearchOptions options) const
{
    QStringList relPaths;
    return findAllResources(type, filter, options, relPaths);
}

QStringList KStandardDirs::findDirs(const char *type, const QString &reldir) const
{
    QStringList result;
    if (QDir::isAbsolutePath(reldir)) {
        if (QDir(reldir).exists())
            result << withSlash(reldir);
        return result;
    }
    foreach (const QString &dir, resourceDirs(type)) {
        const QString full = withSlash(dir + reldir);
        if (QDir(full).exists())
            result << full;
    }
    return result;
}

QString KStandardDirs::saveLocation(const char *type, const QString &suffix, bool create) const
{
    QString dir = userDirFor(d, QByteArray(type));
    if (dir.isEmpty())
        dir = withSlash(QDir::tempPath());
    QString full = withSlash(dir + suffix);
    if (create && !QDir(full).exists())
        makeDir(full);
    return full;
}

QString KStandardDirs::relativeLocation(const char *type, const QString &absPath) const
{
    const QString clean = QDir::cleanPath(absPath);
    foreach (const QString &dir, resourceDirs(type)) {
        if (clean.startsWith(dir))
            return clean.mid(dir.length());
        if (dir.endsWith(QLatin1Char('/')) && clean == dir.left(dir.length() - 1))
            return QString();
    }
    return absPath;
}

QString KStandardDirs::locate(const char *type, const QString &filename)
{
    return KGlobal::dirs()->findResource(type, filename);
}

QString KStandardDirs::locate(const char *type, const QString &filename, const KComponentData &cData)
{
    return cData.dirs()->findResource(type, filename);
}

QString KStandardDirs::locateLocal(const char *type, const QString &filename)
{
    return locateLocal(type, filename, true, KGlobal::mainComponent());
}

QString KStandardDirs::locateLocal(const char *type, const QString &filename, const KComponentData &cData)
{
    return locateLocal(type, filename, true, cData);
}

QString KStandardDirs::locateLocal(const char *type, const QString &filename, bool createDir)
{
    return locateLocal(type, filename, createDir, KGlobal::mainComponent());
}

QString KStandardDirs::locateLocal(const char *type, const QString &filename, bool createDir, const KComponentData &cData)
{
    QString dirPart;
    QString filePart = filename;
    const int slash = filename.lastIndexOf(QLatin1Char('/'));
    if (slash >= 0) {
        dirPart = filename.left(slash + 1);
        filePart = filename.mid(slash + 1);
    }
    return cData.dirs()->saveLocation(type, dirPart, createDir) + filePart;
}

bool KStandardDirs::makeDir(const QString &dir, int)
{
    if (dir.isEmpty())
        return false;
    return QDir().mkpath(dir);
}

bool KStandardDirs::exists(const QString &fullPath)
{
    KDE_struct_stat buff;
    if (KDE::stat(fullPath, &buff) == 0) {
        if (!S_ISDIR(buff.st_mode))
            return true;
        return fullPath.endsWith(QLatin1Char('/'));
    }
    return false;
}

QStringList KStandardDirs::systemPaths(const QString &pstr)
{
    QString p = pstr;
    if (p.isEmpty())
        p = QString::fromLocal8Bit(qgetenv("PATH"));
    QStringList result;
    foreach (const QString &s, p.split(QLatin1Char(':'), QString::SkipEmptyParts))
        result << QDir::cleanPath(s);
    return result;
}

QString KStandardDirs::findExe(const QString &appname, const QString &pathstr, SearchOptions options)
{
    if (QDir::isAbsolutePath(appname)) {
        QFileInfo fi(appname);
        if (fi.exists() && ((options & IgnoreExecBit) || fi.isExecutable()))
            return appname;
        return QString();
    }
    foreach (const QString &dir, systemPaths(pathstr)) {
        const QString full = dir + QLatin1Char('/') + appname;
        QFileInfo fi(full);
        if (fi.exists() && !fi.isDir() && ((options & IgnoreExecBit) || fi.isExecutable()))
            return full;
    }
    return QString();
}

QString KStandardDirs::realPath(const QString &dirname)
{
    const QString canonical = QFileInfo(dirname).canonicalFilePath();
    if (canonical.isEmpty())
        return dirname;
    return withSlash(canonical);
}

QString KStandardDirs::realFilePath(const QString &filename)
{
    const QString canonical = QFileInfo(filename).canonicalFilePath();
    return canonical.isEmpty() ? filename : canonical;
}

QString KStandardDirs::installPath(const char *type)
{
    const QStringList dirs = systemDirsFor(KGlobal::dirs()->d, QByteArray(type));
    return dirs.isEmpty() ? QString() : dirs.first();
}

QString KStandardDirs::kfsstnd_defaultprefix()
{
    return installPath("data");
}

bool KStandardDirs::checkAccess(const QString &pathname, int mode)
{
    if (KDE::access(pathname, mode) == 0)
        return true;
    if (!(mode & W_OK))
        return false;
    /* file does not exist yet: check the directory */
    if (QFile::exists(pathname))
        return false;
    QString dir = pathname;
    const int slash = dir.lastIndexOf(QLatin1Char('/'));
    if (slash < 0)
        dir = QLatin1String(".");
    else if (slash == 0)
        dir = QLatin1String("/");
    else
        dir.truncate(slash);
    return KDE::access(dir, W_OK) == 0;
}
