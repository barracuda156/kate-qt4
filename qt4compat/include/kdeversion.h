/* qt4compat: kdeversion.h */
#ifndef KDECOMPAT_KDEVERSION_H
#define KDECOMPAT_KDEVERSION_H

#include <kdemacros.h>

#define KDE_VERSION_STRING "4.14.3"
#define KDE_VERSION_MAJOR 4
#define KDE_VERSION_MINOR 14
#define KDE_VERSION_RELEASE 3

#define KDE_MAKE_VERSION(a,b,c) (((a) << 16) | ((b) << 8) | (c))
#define KDE_VERSION KDE_MAKE_VERSION(KDE_VERSION_MAJOR, KDE_VERSION_MINOR, KDE_VERSION_RELEASE)
#define KDE_IS_VERSION(a,b,c) (KDE_VERSION >= KDE_MAKE_VERSION(a,b,c))

namespace KDE
{
    inline unsigned int version() { return KDE_VERSION; }
    inline unsigned int versionMajor() { return KDE_VERSION_MAJOR; }
    inline unsigned int versionMinor() { return KDE_VERSION_MINOR; }
    inline unsigned int versionRelease() { return KDE_VERSION_RELEASE; }
    inline const char *versionString() { return KDE_VERSION_STRING; }
}

#endif
