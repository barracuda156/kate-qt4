/* qt4compat: kiconloader.cpp */
#include <kiconloader.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QStyle>

class KIconLoaderPrivate
{
public:
    QStringList extraPaths;
};

static int groupSize(KIconLoader::Group group)
{
    switch (group) {
    case KIconLoader::Small: return KIconLoader::SizeSmall;
    case KIconLoader::Toolbar: return KIconLoader::SizeSmallMedium;
    case KIconLoader::MainToolbar: return KIconLoader::SizeSmallMedium;
    case KIconLoader::Desktop: return KIconLoader::SizeMedium;
    case KIconLoader::Panel: return KIconLoader::SizeMedium;
    case KIconLoader::Dialog: return KIconLoader::SizeMedium;
    default: return KIconLoader::SizeSmall;
    }
}

static void registerThemePaths(const QStringList &extra)
{
    QStringList paths = QIcon::themeSearchPaths();
    bool changed = false;
    QStringList candidates;
    candidates << QLatin1String(":/icons");
    if (KGlobal::hasMainComponent() || QCoreApplication::instance())
        candidates << KGlobal::dirs()->resourceDirs("icon");
    candidates << extra;
    foreach (const QString &c, candidates) {
        QString p = c;
        while (p.length() > 1 && p.endsWith(QLatin1Char('/')))
            p.chop(1);
        if (!paths.contains(p) && (p.startsWith(QLatin1Char(':')) || QDir(p).exists())) {
            paths.prepend(p);
            changed = true;
        }
    }
    if (changed)
        QIcon::setThemeSearchPaths(paths);
    if (QIcon::themeName().isEmpty() || !QDir(QLatin1String(":/icons/") + QIcon::themeName()).exists()) {
        /* pick the first bundled theme (the build system installs exactly one) */
        const QStringList themes = QDir(QLatin1String(":/icons")).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        if (!themes.isEmpty() && QIcon::themeName().isEmpty())
            QIcon::setThemeName(themes.first());
        else if (!themes.isEmpty() && !QIcon::hasThemeIcon(QLatin1String("document-open")))
            QIcon::setThemeName(themes.first());
    }
}

KIconLoader::KIconLoader(const QString &, const QStringList &extraSearchPaths, QObject *)
    : d(new KIconLoaderPrivate)
{
    d->extraPaths = extraSearchPaths;
    registerThemePaths(extraSearchPaths);
}

KIconLoader::~KIconLoader()
{
    delete d;
}

KIconLoader *KIconLoader::global()
{
    static KIconLoader *loader = 0;
    if (!loader)
        loader = new KIconLoader;
    return loader;
}

void KIconLoader::addAppDir(const QString &appname)
{
    Q_UNUSED(appname)
}

QPixmap KIconLoader::loadIcon(const QString &name, Group group, int size, int state, const QStringList &,
                              QString *path_store, bool canReturnNull) const
{
    if (size <= 0)
        size = groupSize(group);
    if (path_store)
        *path_store = iconPath(name, size, true);
    QIcon icon;
    if (QDir::isAbsolutePath(name) && QFile::exists(name))
        icon = QIcon(name);
    else
        icon = QIcon::fromTheme(name);
    if (icon.isNull()) {
        if (canReturnNull)
            return QPixmap();
        icon = QIcon::fromTheme(QLatin1String("unknown"));
        if (icon.isNull())
            return unknown();
    }
    QIcon::Mode mode = QIcon::Normal;
    if (state == DisabledState)
        mode = QIcon::Disabled;
    else if (state == ActiveState)
        mode = QIcon::Active;
    return icon.pixmap(size, size, mode);
}

QPixmap KIconLoader::loadMimeTypeIcon(const QString &iconName, Group group, int size, int state,
                                      const QStringList &overlays, QString *path_store) const
{
    QPixmap pm = loadIcon(iconName, group, size, state, overlays, path_store, true);
    if (pm.isNull())
        pm = loadIcon(QLatin1String("text-x-generic"), group, size, state, overlays, path_store, false);
    return pm;
}

QString KIconLoader::iconPath(const QString &name, int group_or_size, bool canReturnNull) const
{
    if (QDir::isAbsolutePath(name))
        return QFile::exists(name) ? name : (canReturnNull ? QString() : name);
    int size = group_or_size;
    if (group_or_size < 0)
        size = groupSize(static_cast<Group>(-group_or_size - 1));
    else if (group_or_size < LastGroup)
        size = groupSize(static_cast<Group>(group_or_size));
    const QString theme = QIcon::themeName();
    static const char *exts[] = { ".png", ".svg", ".svgz", ".xpm", 0 };
    static const char *contexts[] = { "actions", "apps", "places", "mimetypes", "status", "devices", "categories", "emblems", "animations", 0 };
    foreach (const QString &base, QIcon::themeSearchPaths()) {
        const QString sizeDir = QString::fromLatin1("%1/%2/%3x%3/").arg(base).arg(theme).arg(size);
        for (int c = 0; contexts[c]; ++c) {
            for (int e = 0; exts[e]; ++e) {
                const QString p = sizeDir + QLatin1String(contexts[c]) + QLatin1Char('/') + name + QLatin1String(exts[e]);
                if (QFile::exists(p))
                    return p;
            }
        }
    }
    return canReturnNull ? QString() : name;
}

QString KIconLoader::moviePath(const QString &, Group, int) const
{
    return QString();
}

QStringList KIconLoader::loadAnimated(const QString &, Group, int) const
{
    return QStringList();
}

QStringList KIconLoader::queryIcons(int, Context) const
{
    return QStringList();
}

QStringList KIconLoader::queryIconsByContext(int, Context) const
{
    return QStringList();
}

bool KIconLoader::hasContext(Context) const
{
    return true;
}

QStringList KIconLoader::queryIconsByDir(const QString &iconsDir) const
{
    return QDir(iconsDir).entryList(QStringList() << QLatin1String("*.png") << QLatin1String("*.svg"), QDir::Files);
}

int KIconLoader::currentSize(Group group) const
{
    return groupSize(group);
}

KIconTheme *KIconLoader::theme() const
{
    return 0;
}

bool KIconLoader::alphaBlending(Group) const
{
    return true;
}

void KIconLoader::reconfigure(const QString &, const QStringList &extraSearchPaths)
{
    d->extraPaths = extraSearchPaths;
    registerThemePaths(extraSearchPaths);
}

QPixmap KIconLoader::unknown()
{
    if (QApplication::instance())
        return QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(16, 16);
    return QPixmap();
}

void KIconLoader::emitChange(Group)
{
}

QPixmap DesktopIcon(const QString &name, int size, int state, const QStringList &overlays)
{
    return KIconLoader::global()->loadIcon(name, KIconLoader::Desktop, size, state, overlays);
}

QPixmap BarIcon(const QString &name, int size, int state, const QStringList &overlays)
{
    return KIconLoader::global()->loadIcon(name, KIconLoader::Toolbar, size, state, overlays);
}

QPixmap SmallIcon(const QString &name, int size, int state, const QStringList &overlays)
{
    return KIconLoader::global()->loadIcon(name, KIconLoader::Small, size, state, overlays);
}

QPixmap MainBarIcon(const QString &name, int size, int state, const QStringList &overlays)
{
    return KIconLoader::global()->loadIcon(name, KIconLoader::MainToolbar, size, state, overlays);
}

QPixmap UserIcon(const QString &name, int state, const QStringList &overlays)
{
    return KIconLoader::global()->loadIcon(name, KIconLoader::User, 0, state, overlays);
}

int IconSize(KIconLoader::Group group)
{
    return groupSize(group);
}
