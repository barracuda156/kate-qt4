/* qt4compat: klibloader.cpp */
#include <klibloader.h>

KLibLoader::KLibLoader()
{
}

KLibLoader *KLibLoader::self()
{
    static KLibLoader instance;
    return &instance;
}

KLibrary *KLibLoader::library(const QString &, QLibrary::LoadHints)
{
    return 0;
}

KPluginFactory *KLibLoader::factory(const QString &, QLibrary::LoadHints)
{
    return 0;
}

void KLibLoader::unloadLibrary(const QString &)
{
}

QString KLibLoader::lastErrorMessage() const
{
    return QLatin1String("Dynamic plugin loading is not available in this build.");
}

QString KLibLoader::findLibrary(const QString &, const KComponentData &)
{
    return QString();
}

QString KLibLoader::errorString(int)
{
    return QLatin1String("Dynamic plugin loading is not available in this build.");
}
