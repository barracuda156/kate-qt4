/* qt4compat: kservice.cpp */
#include <kservice.h>

class KServicePrivate
{
public:
    QString name, exec, icon, path;
};

KService::KService(const QString &name, const QString &exec, const QString &icon)
    : QSharedData(), d(new KServicePrivate)
{
    d->name = name;
    d->exec = exec;
    d->icon = icon;
}

KService::KService(const QString &fullpath)
    : QSharedData(), d(new KServicePrivate)
{
    d->path = fullpath;
}

KService::~KService()
{
    delete d;
}

bool KService::isValid() const { return !d->name.isEmpty() && !d->exec.isEmpty(); }
bool KService::isApplication() const { return !d->exec.isEmpty(); }
QString KService::name() const { return d->name; }
QString KService::exec() const { return d->exec; }
QString KService::library() const { return QString(); }
QString KService::icon() const { return d->icon; }
bool KService::terminal() const { return false; }
QString KService::terminalOptions() const { return QString(); }
QString KService::desktopEntryName() const { return d->name.toLower(); }
QString KService::desktopEntryPath() const { return d->path; }
QString KService::entryPath() const { return d->path; }
QString KService::menuId() const { return QString(); }
QString KService::storageId() const { return d->path; }
QString KService::path() const { return QString(); }
QString KService::comment() const { return QString(); }
QString KService::genericName() const { return QString(); }
QString KService::untranslatedGenericName() const { return QString(); }
QStringList KService::keywords() const { return QStringList(); }
QStringList KService::categories() const { return QStringList(); }
QStringList KService::mimeTypes() const { return QStringList(); }
QStringList KService::serviceTypes() const { return QStringList(); }
bool KService::hasServiceType(const QString &) const { return false; }
bool KService::hasMimeType(const QString &) const { return false; }
bool KService::allowAsDefault() const { return false; }
bool KService::allowMultipleFiles() const { return true; }
int KService::initialPreference() const { return 1; }
bool KService::noDisplay() const { return true; }
bool KService::showInKDE() const { return true; }
QString KService::parentApp() const { return QString(); }
QString KService::pluginKeyword() const { return QString(); }
QString KService::docPath() const { return QString(); }
QVariant KService::property(const QString &, QVariant::Type) const { return QVariant(); }
QStringList KService::propertyNames() const { return QStringList(); }
KService::DBusStartupType KService::dbusStartupType() const { return DBusNone; }
QString KService::locateLocal() const { return QString(); }

KService::Ptr KService::serviceByName(const QString &) { return Ptr(); }
KService::Ptr KService::serviceByDesktopPath(const QString &) { return Ptr(); }
KService::Ptr KService::serviceByDesktopName(const QString &) { return Ptr(); }
KService::Ptr KService::serviceByMenuId(const QString &) { return Ptr(); }
KService::Ptr KService::serviceByStorageId(const QString &) { return Ptr(); }
KService::List KService::allServices() { return List(); }
