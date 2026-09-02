/* qt4compat: kiconloader.h - icon lookup through QIcon::fromTheme.
 * The bundled theme lives in the qt4compat resources (":/icons"); see
 * KIconLoader::global() for how search paths are registered. */
#ifndef KDECOMPAT_KICONLOADER_H
#define KDECOMPAT_KICONLOADER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <kdemacros.h>

class KComponentData;
class KIconTheme;
class KIconLoaderPrivate;

class KDEUI_EXPORT KIconLoader
{
public:
    enum Context { Any, Action, Application, Device, FileSystem, MimeType, Animation, Category, Emblem, Emote, International, Place, StatusIcon };
    enum Type { Fixed, Scalable, Threshold };
    enum MatchType { MatchExact, MatchBest };
    enum Group { NoGroup = -1, Desktop = 0, FirstGroup = 0, Toolbar, MainToolbar, Small, Panel, Dialog, LastGroup, User };
    enum StdSizes { SizeSmall = 16, SizeSmallMedium = 22, SizeMedium = 32, SizeLarge = 48, SizeHuge = 64, SizeEnormous = 128 };
    enum States { DefaultState, ActiveState, DisabledState, LastState };

    explicit KIconLoader(const QString &appname = QString(), const QStringList &extraSearchPaths = QStringList(), QObject *parent = 0);
    ~KIconLoader();

    static KIconLoader *global();

    void addAppDir(const QString &appname);
    QPixmap loadIcon(const QString &name, Group group, int size = 0, int state = DefaultState,
                     const QStringList &overlays = QStringList(), QString *path_store = 0, bool canReturnNull = false) const;
    QPixmap loadMimeTypeIcon(const QString &iconName, Group group, int size = 0, int state = DefaultState,
                             const QStringList &overlays = QStringList(), QString *path_store = 0) const;
    QString iconPath(const QString &name, int group_or_size, bool canReturnNull = false) const;
    QString moviePath(const QString &name, Group group, int size = 0) const;
    QStringList loadAnimated(const QString &name, Group group, int size = 0) const;
    QStringList queryIcons(int group_or_size, Context context = Any) const;
    QStringList queryIconsByContext(int group_or_size, Context context = Any) const;
    bool hasContext(Context context) const;
    QStringList queryIconsByDir(const QString &iconsDir) const;
    int currentSize(Group group) const;
    KIconTheme *theme() const;
    bool alphaBlending(Group group) const;
    void reconfigure(const QString &appname, const QStringList &extraSearchPaths);
    static QPixmap unknown();
    static void emitChange(Group group);

private:
    KIconLoaderPrivate *const d;
};

KDEUI_EXPORT QPixmap DesktopIcon(const QString &name, int size = 0, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());
KDEUI_EXPORT QPixmap BarIcon(const QString &name, int size = 0, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());
KDEUI_EXPORT QPixmap SmallIcon(const QString &name, int size = 0, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());
KDEUI_EXPORT QPixmap MainBarIcon(const QString &name, int size = 0, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());
KDEUI_EXPORT QPixmap UserIcon(const QString &name, int state = KIconLoader::DefaultState, const QStringList &overlays = QStringList());
KDEUI_EXPORT int IconSize(KIconLoader::Group group);

#endif
