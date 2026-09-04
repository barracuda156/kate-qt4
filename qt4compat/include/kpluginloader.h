/* qt4compat: kpluginloader.h - no dynamic plugin loading. Plugins are linked
 * into the application and registered by name:
 *
 *   * the plugin's sources use K_PLUGIN_FACTORY/K_EXPORT_PLUGIN as usual and
 *     are compiled with KDE_STATIC_PLUGIN_NAME=<name> (kate_add_static_plugin()
 *     in cmake/KateQt4Macros.cmake), which makes K_EXPORT_PLUGIN emit
 *     kde_static_plugin_create_<name>(), see kexportplugin.h;
 *   * one translation unit of the executable itself contains
 *     K_IMPORT_PLUGIN(<name>): a static registrar object there calls
 *     registerStaticPlugin() before main() (it must live in the executable's
 *     own sources, not in a static library, or the linker drops it);
 *   * KPluginLoader(<name>).factory() then hands out the factory, which is
 *     created once per process and kept, like a loaded shared object.
 *
 * <name> is the X-KDE-Library value of the plugin's .desktop file. */
#ifndef KDECOMPAT_KPLUGINLOADER_H
#define KDECOMPAT_KPLUGINLOADER_H

#include <QtCore/QLibrary>
#include <QtCore/QString>
#include <kdemacros.h>
#include <kcomponentdata.h>
#include <kexportplugin.h>

class KPluginFactory;
class KService;
class KPluginLoaderPrivate;

class KDECORE_EXPORT KPluginLoader : public QLibrary
{
public:
    typedef KPluginFactory *(*FactoryCreator)();

    explicit KPluginLoader(const QString &plugin, const KComponentData &componentdata = KComponentData(), QObject *parent = 0);
    explicit KPluginLoader(const KService &service, const KComponentData &componentdata = KComponentData(), QObject *parent = 0);
    ~KPluginLoader();

    KPluginFactory *factory();
    QString pluginName() const;
    quint32 pluginVersion() const;
    QString errorString() const;
    bool isLoaded() const;
    bool load();

    static QString findPlugin(const QString &name, const KComponentData &componentdata = KComponentData());

    /* qt4compat */
    static void registerStaticPlugin(const QString &name, FactoryCreator creator);
    static bool hasStaticPlugin(const QString &name);
    static QStringList staticPlugins();

private:
    KPluginLoaderPrivate *const d;
};

/* qt4compat: what K_IMPORT_PLUGIN instantiates */
class KDECORE_EXPORT KStaticPluginRegistrar
{
public:
    KStaticPluginRegistrar(const char *name, KPluginLoader::FactoryCreator creator);
};

#define K_IMPORT_PLUGIN(name) \
    KPluginFactory *KDECOMPAT_STATIC_PLUGIN_CREATOR(name)(); \
    static const KStaticPluginRegistrar KDECOMPAT_PASTE(kde_static_plugin_registrar_, name)(#name, &KDECOMPAT_STATIC_PLUGIN_CREATOR(name));

#endif
