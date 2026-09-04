/* qt4compat: kexportplugin.h - plugins are linked statically.
 *
 * In kdelibs K_EXPORT_PLUGIN(factory) turns a translation unit into a
 * dynamically loadable plugin. Here nothing is loaded from disk: when the
 * plugin's build defines KDE_STATIC_PLUGIN_NAME=<library name> (the CMake
 * helper kate_add_static_plugin() does) the macro defines a factory creator
 *
 *     KPluginFactory *kde_static_plugin_create_<name>()
 *
 * that the application registers with K_IMPORT_PLUGIN(<name>) (see
 * kpluginloader.h), Q_IMPORT_PLUGIN style. <name> must match the
 * X-KDE-Library entry of the plugin's .desktop file, since that is what
 * KService::createInstance() and KPluginLoader look up. Without
 * KDE_STATIC_PLUGIN_NAME the macro expands to nothing and the sources merely
 * compile (katepart's KateFactory is such a case). */
#ifndef KDECOMPAT_KEXPORTPLUGIN_H
#define KDECOMPAT_KEXPORTPLUGIN_H

#include <QtCore/QtPlugin>
#include <kdemacros.h>

class KPluginFactory;

#define KDECOMPAT_PASTE2(a, b) a##b
#define KDECOMPAT_PASTE(a, b) KDECOMPAT_PASTE2(a, b)
#define KDECOMPAT_STATIC_PLUGIN_CREATOR(name) KDECOMPAT_PASTE(kde_static_plugin_create_, name)

#define K_EXPORT_PLUGIN_VERSION(version) \
    static const quint32 kde_plugin_version = version;

#ifdef KDE_STATIC_PLUGIN_NAME
#define K_EXPORT_PLUGIN(factory) \
    KPluginFactory *KDECOMPAT_STATIC_PLUGIN_CREATOR(KDE_STATIC_PLUGIN_NAME)() \
    { \
        return new factory; \
    }
#else
#define K_EXPORT_PLUGIN(factory)
#endif

#endif
