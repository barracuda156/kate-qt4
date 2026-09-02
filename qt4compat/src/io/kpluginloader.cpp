/* qt4compat: kpluginloader.cpp */
#include <kpluginloader.h>

#include <kpluginfactory.h>
#include <kservice.h>

#include <QtCore/QHash>

namespace {
static QHash<QString, KPluginLoader::FactoryCreator> *creators()
{
    static QHash<QString, KPluginLoader::FactoryCreator> *h = new QHash<QString, KPluginLoader::FactoryCreator>;
    return h;
}
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
    QHash<QString, FactoryCreator>::const_iterator it = creators()->constFind(d->name);
    if (it == creators()->constEnd())
        return 0;
    d->factory = it.value()();
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
    creators()->insert(name, creator);
}

bool KPluginLoader::hasStaticPlugin(const QString &name)
{
    return creators()->contains(name);
}
