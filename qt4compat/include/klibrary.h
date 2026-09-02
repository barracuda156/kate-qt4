/* qt4compat: klibrary.h - no shared libraries are loaded at runtime */
#ifndef KDECOMPAT_KLIBRARY_H
#define KDECOMPAT_KLIBRARY_H

#include <QtCore/QLibrary>
#include <QtCore/QString>
#include <kdemacros.h>
#include <kcomponentdata.h>

class KPluginFactory;

class KDECORE_EXPORT KLibrary : public QLibrary
{
public:
    typedef void (*void_function_ptr)();

    explicit KLibrary(QObject *parent = 0);
    explicit KLibrary(const QString &name, const KComponentData &cData = KComponentData(), QObject *parent = 0);
    KLibrary(const QString &name, int verNum, const KComponentData &cData = KComponentData(), QObject *parent = 0);
    virtual ~KLibrary();

    KPluginFactory *factory(const char *factoryname = 0);
    void_function_ptr resolveFunction(const char *name);
    void setFileName(const QString &name, const KComponentData &data = KComponentData());
    bool load();
    bool isLoaded() const;
};

#endif
