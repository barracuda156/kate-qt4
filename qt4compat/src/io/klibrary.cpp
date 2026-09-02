/* qt4compat: klibrary.cpp */
#include <klibrary.h>

KLibrary::KLibrary(QObject *parent)
    : QLibrary(parent)
{
}

KLibrary::KLibrary(const QString &name, const KComponentData &, QObject *parent)
    : QLibrary(parent)
{
    QLibrary::setFileName(name);
}

KLibrary::KLibrary(const QString &name, int, const KComponentData &, QObject *parent)
    : QLibrary(parent)
{
    QLibrary::setFileName(name);
}

KLibrary::~KLibrary()
{
}

KPluginFactory *KLibrary::factory(const char *)
{
    return 0;
}

KLibrary::void_function_ptr KLibrary::resolveFunction(const char *)
{
    return 0;
}

void KLibrary::setFileName(const QString &name, const KComponentData &)
{
    QLibrary::setFileName(name);
}

bool KLibrary::load()
{
    return false;
}

bool KLibrary::isLoaded() const
{
    return false;
}
