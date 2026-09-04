/* qt4compat: kservice.cpp */
#include <kservice.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kstandarddirs.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>

class KServicePrivate
{
public:
    KServicePrivate() : valid(false) {}

    void parse(const QString &fullpath);

    bool valid;
    QString name, exec, icon, path, type, library, keyword, comment, genericName;
    QStringList serviceTypes, mimeTypes, keywords, categories;
    QMap<QString, QString> properties;
};

static QStringList xdgList(const QString &value)
{
    return value.split(QLatin1Char(';'), QString::SkipEmptyParts);
}

void KServicePrivate::parse(const QString &fullpath)
{
    path = fullpath;
    KConfig config(fullpath, KConfig::SimpleConfig);
    if (!config.hasGroup("Desktop Entry")) {
        kWarning() << "not a desktop file:" << fullpath;
        return;
    }
    const KConfigGroup g(&config, "Desktop Entry");
    properties = g.entryMap();
    type = g.readEntry("Type", QString());
    name = g.readEntry("Name", QString());
    exec = g.readEntry("Exec", QString());
    icon = g.readEntry("Icon", QString());
    comment = g.readEntry("Comment", QString());
    genericName = g.readEntry("GenericName", QString());
    library = g.readEntry("X-KDE-Library", QString());
    keyword = g.readEntry("X-KDE-PluginKeyword", QString());
    /* ServiceTypes is comma separated in kdelibs, the XDG lists use ';' */
    serviceTypes = g.readEntry("ServiceTypes", QStringList()) + g.readEntry("X-KDE-ServiceTypes", QStringList());
    mimeTypes = xdgList(g.readEntry("MimeType", QString()));
    keywords = xdgList(g.readEntry("Keywords", QString()));
    categories = xdgList(g.readEntry("Categories", QString()));
    valid = !name.isEmpty() && !type.isEmpty();
}

KService::KService(const QString &name, const QString &exec, const QString &icon)
    : QSharedData(), d(new KServicePrivate)
{
    d->name = name;
    d->exec = exec;
    d->icon = icon;
    d->type = QLatin1String("Application");
    d->valid = !name.isEmpty();
}

KService::KService(const QString &fullpath)
    : QSharedData(), d(new KServicePrivate)
{
    d->parse(fullpath);
}

KService::~KService()
{
    delete d;
}

bool KService::isValid() const { return d->valid; }
bool KService::isApplication() const { return d->type == QLatin1String("Application"); }
QString KService::name() const { return d->name; }
QString KService::exec() const { return d->exec; }
QString KService::library() const { return d->library; }
QString KService::icon() const { return d->icon; }
bool KService::terminal() const { return property(QLatin1String("Terminal"), QVariant::Bool).toBool(); }
QString KService::terminalOptions() const { return d->properties.value(QLatin1String("TerminalOptions")); }

QString KService::desktopEntryName() const
{
    if (d->path.isEmpty())
        return d->name.toLower();
    QString base = QFileInfo(d->path).fileName();
    if (base.endsWith(QLatin1String(".desktop")))
        base.chop(8);
    return base;
}

QString KService::desktopEntryPath() const { return d->path; }
QString KService::entryPath() const { return d->path; }
QString KService::menuId() const { return storageId(); }
QString KService::storageId() const { return d->path.isEmpty() ? QString() : QFileInfo(d->path).fileName(); }
QString KService::path() const { return d->properties.value(QLatin1String("Path")); }
QString KService::comment() const { return d->comment; }
QString KService::genericName() const { return d->genericName; }
QString KService::untranslatedGenericName() const { return d->genericName; }
QStringList KService::keywords() const { return d->keywords; }
QStringList KService::categories() const { return d->categories; }
QStringList KService::mimeTypes() const { return d->mimeTypes; }
QStringList KService::serviceTypes() const { return d->serviceTypes; }
bool KService::hasServiceType(const QString &serviceType) const { return d->serviceTypes.contains(serviceType); }
bool KService::hasMimeType(const QString &mimeType) const { return d->mimeTypes.contains(mimeType); }
bool KService::allowAsDefault() const { return !property(QLatin1String("AllowDefault"), QVariant::Bool).isValid() || property(QLatin1String("AllowDefault"), QVariant::Bool).toBool(); }
bool KService::allowMultipleFiles() const { return d->exec.contains(QLatin1String("%F")) || d->exec.contains(QLatin1String("%U")); }
int KService::initialPreference() const { return property(QLatin1String("InitialPreference"), QVariant::Int).toInt(); }
bool KService::noDisplay() const { return property(QLatin1String("NoDisplay"), QVariant::Bool).toBool(); }
bool KService::showInKDE() const { return true; }
QString KService::parentApp() const { return d->properties.value(QLatin1String("X-KDE-ParentApp")); }
QString KService::pluginKeyword() const { return d->keyword; }
QString KService::docPath() const { return d->properties.value(QLatin1String("X-DocPath")); }

QVariant KService::property(const QString &name, QVariant::Type t) const
{
    QVariant v;
    if (name == QLatin1String("Name"))
        v = d->name;
    else if (name == QLatin1String("Exec"))
        v = d->exec;
    else if (name == QLatin1String("Icon"))
        v = d->icon;
    else if (name == QLatin1String("Type"))
        v = d->type;
    else if (name == QLatin1String("Comment"))
        v = d->comment;
    else if (name == QLatin1String("GenericName"))
        v = d->genericName;
    else if (name == QLatin1String("Library") || name == QLatin1String("X-KDE-Library"))
        v = d->library;
    else if (name == QLatin1String("ServiceTypes"))
        v = d->serviceTypes;
    else if (name == QLatin1String("MimeType"))
        v = d->mimeTypes;
    else if (name == QLatin1String("DesktopEntryPath"))
        v = d->path;
    else if (name == QLatin1String("DesktopEntryName"))
        v = desktopEntryName();
    else if (d->properties.contains(name))
        v = d->properties.value(name);
    else
        return QVariant();
    if (t != QVariant::Invalid && v.type() != t) {
        if (t == QVariant::Bool && v.type() == QVariant::String) {
            /* "true"/"True"/"1" as in kdelibs' KDesktopFile */
            const QString s = v.toString().toLower();
            v = QVariant(s == QLatin1String("true") || s == QLatin1String("1"));
        } else if (!v.convert(t)) {
            return QVariant();
        }
    }
    return v;
}

QStringList KService::propertyNames() const { return d->properties.keys(); }
KService::DBusStartupType KService::dbusStartupType() const { return DBusNone; }
QString KService::locateLocal() const { return QString(); }

QObject *KService::createInstanceInternal(const char *iface, QWidget *parentWidget, QObject *parent,
                                          const QVariantList &args, QString *error) const
{
    if (d->library.isEmpty()) {
        if (error)
            *error = QLatin1String("Service '") + d->name + QLatin1String("' does not name a plugin library.");
        return 0;
    }
    KPluginLoader loader(*this);
    KPluginFactory *factory = loader.factory();
    if (!factory) {
        if (error)
            *error = loader.errorString();
        return 0;
    }
    QObject *o = factory->create(iface, parentWidget, parent, args, d->keyword);
    if (!o && error)
        *error = QLatin1String("The plugin '") + d->library + QLatin1String("' does not provide '")
                 + QString::fromLatin1(iface) + QLatin1String("' with keyword '") + d->keyword + QLatin1Char('\'');
    return o;
}

/* every services/*.desktop in the resource dirs; user files override system
 * ones of the same relative name (KStandardDirs::NoDuplicates) */
KService::List KService::allServices()
{
    static List services;
    static bool scanned = false;
    if (scanned)
        return services;
    scanned = true;
    const QStringList files = KGlobal::dirs()->findAllResources("services", QLatin1String("*.desktop"),
                                                                KStandardDirs::Recursive | KStandardDirs::NoDuplicates);
    foreach (const QString &file, files) {
        Ptr s(new KService(file));
        if (s->isValid())
            services << s;
        else
            kWarning() << "ignoring invalid service file" << file;
    }
    return services;
}

KService::Ptr KService::serviceByName(const QString &name)
{
    foreach (const Ptr &s, allServices())
        if (s->name() == name)
            return s;
    return Ptr();
}

KService::Ptr KService::serviceByDesktopPath(const QString &path)
{
    foreach (const Ptr &s, allServices())
        if (s->entryPath() == path || s->storageId() == path)
            return s;
    return Ptr();
}

KService::Ptr KService::serviceByDesktopName(const QString &name)
{
    const QString lower = name.toLower();
    foreach (const Ptr &s, allServices())
        if (s->desktopEntryName().toLower() == lower)
            return s;
    return Ptr();
}

KService::Ptr KService::serviceByMenuId(const QString &menuId)
{
    return serviceByStorageId(menuId);
}

KService::Ptr KService::serviceByStorageId(const QString &storageId)
{
    foreach (const Ptr &s, allServices())
        if (s->storageId() == storageId)
            return s;
    if (storageId.endsWith(QLatin1String(".desktop")))
        return serviceByDesktopName(storageId.left(storageId.length() - 8));
    return serviceByDesktopName(storageId);
}
