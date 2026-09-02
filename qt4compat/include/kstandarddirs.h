/* qt4compat: kstandarddirs.h - resource lookup without a KDE install tree.
 *
 * Resource types and where they resolve to:
 *   "data"    : <system data dirs>/<relpath>, user: <userDataDir>/<relpath>
 *   "appdata" : "data" + "<componentName>/"
 *   "config"  : <system data dirs>/config/, user: <userConfigDir>/
 *   "tmp"     : QDir::tempPath()
 *   "cache"   : <userDataDir>/cache/
 *   "icon"    : <system data dirs>/icons/
 *   "locale"  : <system data dirs>/locale/
 *   "lib","exe","module","libexec": the application directory
 *   others    : <system data dirs>/<type>/
 *
 * System data dirs, in priority order: $KATE_RESOURCE_DIRS (colon separated),
 * <app bundle>/Contents/Resources (macOS), KATE_INSTALL_DATADIR (compile
 * time), <appdir>/../share/kate-qt4, <appdir>/share.
 * User dirs: macOS ~/Library/Application Support/Kate and
 * ~/Library/Preferences/Kate; elsewhere $XDG_DATA_HOME/kate-qt4 and
 * $XDG_CONFIG_HOME/kate-qt4. */
#ifndef KDECOMPAT_KSTANDARDDIRS_H
#define KDECOMPAT_KSTANDARDDIRS_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>

class KComponentData;
class KStandardDirsPrivate;

class KDECORE_EXPORT KStandardDirs
{
public:
    enum SearchOption {
        NoSearchOptions = 0,
        Recursive = 1,
        NoDuplicates = 2,
        IgnoreExecBit = 4
    };
    Q_DECLARE_FLAGS(SearchOptions, SearchOption)

    KStandardDirs();
    virtual ~KStandardDirs();

    void addPrefix(const QString &dir);
    void addXdgConfigPrefix(const QString &dir);
    void addXdgDataPrefix(const QString &dir);
    bool addResourceType(const char *type, const char *basetype, const QString &relativename, bool priority = true);
    bool addResourceDir(const char *type, const QString &absdir, bool priority = true);

    QString findResource(const char *type, const QString &filename) const;
    QStringList findAllResources(const char *type, const QString &filter = QString(),
                                 SearchOptions options = NoSearchOptions) const;
    QStringList findAllResources(const char *type, const QString &filter, SearchOptions options,
                                 QStringList &relPaths) const;
    QString findResourceDir(const char *type, const QString &filename) const;
    QStringList findDirs(const char *type, const QString &reldir) const;
    QString saveLocation(const char *type, const QString &suffix = QString(), bool create = true) const;
    QString relativeLocation(const char *type, const QString &absPath) const;
    QStringList resourceDirs(const char *type) const;
    QStringList allTypes() const;
    QString localkdedir() const;
    QString localxdgdatadir() const;
    QString localxdgconfdir() const;

    static QString locate(const char *type, const QString &filename);
    static QString locate(const char *type, const QString &filename, const KComponentData &cData);
    static QString locateLocal(const char *type, const QString &filename);
    static QString locateLocal(const char *type, const QString &filename, const KComponentData &cData);
    static QString locateLocal(const char *type, const QString &filename, bool createDir);
    static QString locateLocal(const char *type, const QString &filename, bool createDir, const KComponentData &cData);
    static bool makeDir(const QString &dir, int mode = 0755);
    static bool exists(const QString &fullPath);
    static QString findExe(const QString &appname, const QString &pathstr = QString(), SearchOptions options = NoSearchOptions);
    static QString realPath(const QString &dirname);
    static QString realFilePath(const QString &filename);
    static QString installPath(const char *type);
    static QString kfsstnd_defaultprefix();
    static QStringList systemPaths(const QString &pstr = QString());
    static bool checkAccess(const QString &pathname, int mode);

    /* qt4compat additions */
    static QString userDataDir();
    static QString userConfigDir();
    static QStringList systemDataDirs();
    void setComponentName(const QString &name);

private:
    KStandardDirsPrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KStandardDirs::SearchOptions)

#endif
