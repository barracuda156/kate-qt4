/* qt4compat: klibloader.h - no dynamic loading */
#ifndef KDECOMPAT_KLIBLOADER_H
#define KDECOMPAT_KLIBLOADER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <kdemacros.h>
#include <kcomponentdata.h>
#include <kpluginfactory.h>
#include <klibrary.h>

class KDECORE_EXPORT KLibLoader
{
public:
    enum ComponentLoadingError {
        ErrNoLibrary = 1,
        ErrNoFactory,
        ErrNoComponent,
        ErrServiceProvidesNoLibrary,
        ErrNoServiceFound
    };

    static KLibLoader *self();

    KLibrary *library(const QString &libname, QLibrary::LoadHints loadHint = 0);
    KPluginFactory *factory(const QString &libname, QLibrary::LoadHints loadHint = 0);
    void unloadLibrary(const QString &libname);
    QString lastErrorMessage() const;
    static QString findLibrary(const QString &libname, const KComponentData &cData = KComponentData());
    static QString errorString(int componentLoadingError);

private:
    KLibLoader();
};

#endif
