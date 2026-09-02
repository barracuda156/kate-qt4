/* qt4compat: kpluginloader.h - no dynamic plugin loading. Factories for
 * statically linked plugins can be registered with registerStaticPlugin(). */
#ifndef KDECOMPAT_KPLUGINLOADER_H
#define KDECOMPAT_KPLUGINLOADER_H

#include <QtCore/QLibrary>
#include <QtCore/QString>
#include <kdemacros.h>
#include <kcomponentdata.h>

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

private:
    KPluginLoaderPrivate *const d;
};

#endif
