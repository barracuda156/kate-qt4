/* qt4compat: kexportplugin.h - plugins are linked statically, the export
 * macros only keep the plugin sources compiling. */
#ifndef KDECOMPAT_KEXPORTPLUGIN_H
#define KDECOMPAT_KEXPORTPLUGIN_H

#include <QtCore/QtPlugin>
#include <kdemacros.h>

#define K_EXPORT_PLUGIN_VERSION(version) \
    static const quint32 kde_plugin_version = version;

#define K_EXPORT_PLUGIN(factory)

#endif
