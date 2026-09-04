/* qt4compat: kpluginloader.cpp */
#include <kpluginloader.h>

#include <kpluginfactory.h>
#include <kservice.h>

#include <QtCore/QHash>

namespace {
struct StaticPlugin
{
    StaticPlugin() : creator(0), factory(0) {}
    KPluginLoader::FactoryCreator creator;
    KPluginFactory *factory; /* created on first use, lives as long as the process */
};

/* heap allocated on first use: K_IMPORT_PLUGIN registrars run during static
 * initialization, before any file-scope object here would be constructed */
static QHash<QString, StaticPlugin> *plugins()
{
    static QHash<QString, StaticPlugin> *h = new QHash<QString, StaticPlugin>;
    return h;
}
}

KStaticPluginRegistrar::KStaticPluginRegistrar(const char *name, KPluginLoader::FactoryCreator creator)
{
    KPluginLoader::registerStaticPlugin(QString::fromLatin1(name), creator);
}

class KPluginLoaderPrivate
{
public:
    KPluginLoaderPrivate() : factory(0) {}
    QString name;
    QString error;
    KPluginFactory *factory;
};

KPluginLoader::KPluginLoader(const QString &plugin, const KComponentData &, QObject *parent)
    : QLibrary(parent), d(new KPluginLoaderPrivate)
{
    d->name = plugin;
    if (!hasStaticPlugin(plugin))
        d->error = QLatin1String("Plugin '") + plugin + QLatin1String("' is not built into this application.");
}

KPluginLoader::KPluginLoader(const KService &service, const KComponentData &, QObject *parent)
    : QLibrary(parent), d(new KPluginLoaderPrivate)
{
    d->name = service.library();
    if (!hasStaticPlugin(d->name))
        d->error = QLatin1String("Plugin '") + d->name + QLatin1String("' is not built into this application.");
}

KPluginLoader::~KPluginLoader()
{
    delete d;
}

KPluginFactory *KPluginLoader::factory()
{
    if (d->factory)
        return d->factory;
    QHash<QString, StaticPlugin>::iterator it = plugins()->find(d->name);
    if (it == plugins()->end())
        return 0;
    if (!it->factory && it->creator)
        it->factory = it->creator();
    d->factory = it->factory;
    return d->factory;
}

QString KPluginLoader::pluginName() const
{
    return d->name;
}

quint32 KPluginLoader::pluginVersion() const
{
    return 0;
}

QString KPluginLoader::errorString() const
{
    return d->error;
}

bool KPluginLoader::isLoaded() const
{
    return d->factory != 0;
}

bool KPluginLoader::load()
{
    return factory() != 0;
}

QString KPluginLoader::findPlugin(const QString &name, const KComponentData &)
{
    return hasStaticPlugin(name) ? name : QString();
}

void KPluginLoader::registerStaticPlugin(const QString &name, FactoryCreator creator)
{
    StaticPlugin &p = (*plugins())[name];
    p.creator = creator;
}

bool KPluginLoader::hasStaticPlugin(const QString &name)
{
    return plugins()->contains(name);
}

QStringList KPluginLoader::staticPlugins()
{
    return plugins()->keys();
}
