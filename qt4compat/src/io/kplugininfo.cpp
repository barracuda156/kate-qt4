/* qt4compat: kplugininfo.cpp */
#include <kplugininfo.h>

#include <QtCore/QSharedData>

class KPluginInfoPrivate : public QSharedData
{
public:
    KPluginInfoPrivate() : enabled(false), enabledByDefault(false), hidden(false), valid(false) {}
    QString name, comment, icon, pluginName, author, email, category, version, website, license, entryPath;
    QStringList dependencies;
    KService::Ptr service;
    KConfigGroup config;
    bool enabled;
    bool enabledByDefault;
    bool hidden;
    bool valid;
};

KPluginInfo::KPluginInfo(const QString &filename, const char *)
    : d(new KPluginInfoPrivate)
{
    d->entryPath = filename;
    d->pluginName = filename.section(QLatin1Char('/'), -1).section(QLatin1Char('.'), 0, 0);
    d->name = d->pluginName;
    d->valid = !filename.isEmpty();
}

KPluginInfo::KPluginInfo(const KService::Ptr service)
    : d(new KPluginInfoPrivate)
{
    d->service = service;
    if (service) {
        d->name = service->name();
        d->pluginName = service->library();
        d->icon = service->icon();
        d->comment = service->comment();
        d->entryPath = service->entryPath();
        d->valid = true;
    }
}

KPluginInfo::KPluginInfo()
    : d(new KPluginInfoPrivate)
{
}

KPluginInfo::KPluginInfo(const KPluginInfo &rhs)
    : d(rhs.d)
{
}

KPluginInfo &KPluginInfo::operator=(const KPluginInfo &rhs)
{
    d = rhs.d;
    return *this;
}

bool KPluginInfo::operator==(const KPluginInfo &rhs) const
{
    return d == rhs.d;
}

bool KPluginInfo::operator<(const KPluginInfo &rhs) const
{
    if (category() < rhs.category())
        return true;
    if (category() == rhs.category())
        return name() < rhs.name();
    return false;
}

bool KPluginInfo::operator>(const KPluginInfo &rhs) const
{
    if (category() > rhs.category())
        return true;
    if (category() == rhs.category())
        return name() > rhs.name();
    return false;
}

KPluginInfo::~KPluginInfo()
{
}

KPluginInfo::List KPluginInfo::fromServices(const KService::List &services, const KConfigGroup &config)
{
    List list;
    foreach (const KService::Ptr &s, services) {
        KPluginInfo info(s);
        info.setConfig(config);
        list << info;
    }
    return list;
}

KPluginInfo::List KPluginInfo::fromFiles(const QStringList &files, const KConfigGroup &config)
{
    List list;
    foreach (const QString &f, files) {
        KPluginInfo info(f);
        info.setConfig(config);
        list << info;
    }
    return list;
}

KPluginInfo::List KPluginInfo::fromKPartsInstanceName(const QString &, const KConfigGroup &)
{
    return List();
}

bool KPluginInfo::isHidden() const { return d->hidden; }
void KPluginInfo::setPluginEnabled(bool enabled) { d->enabled = enabled; }
bool KPluginInfo::isPluginEnabled() const { return d->enabled; }
bool KPluginInfo::isPluginEnabledByDefault() const { return d->enabledByDefault; }
QVariant KPluginInfo::property(const QString &) const { return QVariant(); }
QString KPluginInfo::name() const { return d->name; }
QString KPluginInfo::comment() const { return d->comment; }
QString KPluginInfo::icon() const { return d->icon; }
QString KPluginInfo::entryPath() const { return d->entryPath; }
QString KPluginInfo::author() const { return d->author; }
QString KPluginInfo::email() const { return d->email; }
QString KPluginInfo::category() const { return d->category; }
QString KPluginInfo::pluginName() const { return d->pluginName; }
QString KPluginInfo::version() const { return d->version; }
QString KPluginInfo::website() const { return d->website; }
QString KPluginInfo::license() const { return d->license; }
QStringList KPluginInfo::dependencies() const { return d->dependencies; }
KService::Ptr KPluginInfo::service() const { return d->service; }
QList<KService::Ptr> KPluginInfo::kcmServices() const { return QList<KService::Ptr>(); }

void KPluginInfo::setConfig(const KConfigGroup &config)
{
    d->config = config;
}

KConfigGroup KPluginInfo::config() const
{
    return d->config;
}

void KPluginInfo::save(KConfigGroup config)
{
    if (!config.isValid())
        config = d->config;
    if (config.isValid())
        config.writeEntry(d->pluginName + QLatin1String("Enabled"), d->enabled);
}

void KPluginInfo::load(const KConfigGroup &config)
{
    KConfigGroup group = config.isValid() ? config : d->config;
    if (group.isValid())
        d->enabled = group.readEntry(d->pluginName + QLatin1String("Enabled"), d->enabledByDefault);
    else
        d->enabled = d->enabledByDefault;
}

void KPluginInfo::defaults()
{
    d->enabled = d->enabledByDefault;
}

bool KPluginInfo::isValid() const
{
    return d->valid;
}

void KPluginInfo::setName(const QString &name) { d->name = name; d->valid = true; }
void KPluginInfo::setPluginName(const QString &pluginName) { d->pluginName = pluginName; d->valid = true; }
void KPluginInfo::setComment(const QString &comment) { d->comment = comment; }
void KPluginInfo::setIcon(const QString &icon) { d->icon = icon; }
void KPluginInfo::setEnabledByDefault(bool on) { d->enabledByDefault = on; }
void KPluginInfo::setDependencies(const QStringList &deps) { d->dependencies = deps; }
