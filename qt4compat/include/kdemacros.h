/* qt4compat: subset of kdelibs' kdemacros.h */
#ifndef KDECOMPAT_KDEMACROS_H
#define KDECOMPAT_KDEMACROS_H

#include <QtCore/qglobal.h>

#define KDE_EXPORT Q_DECL_EXPORT
#define KDE_IMPORT Q_DECL_IMPORT
#define KDE_NO_EXPORT
#define KDE_DEPRECATED Q_DECL_DEPRECATED
#define KDE_CONSTRUCTOR_DEPRECATED
#define KDE_DEPRECATED_EXPORT KDE_EXPORT KDE_DEPRECATED
#define KDE_MUST_USE_RESULT
#define KDE_PACKED
#define KDE_WEAK_SYMBOL
#define KDE_NO_DEPRECATED

#define KDE_ISLIKELY(x)   Q_LIKELY(x)
#define KDE_ISUNLIKELY(x) Q_UNLIKELY(x)

#define KDE_DUMMY_QHASH_FUNCTION(C) \
    inline uint qHash(const C) { qWarning("Dummy qHash for " #C); return 0; }

/* everything in this compat layer is built into one static library */
#define KDECORE_EXPORT
#define KDEUI_EXPORT
#define KPARTS_EXPORT
#define KIO_EXPORT
#define KFILE_EXPORT
#define KCMUTILS_EXPORT
#define KUTILS_EXPORT
#define SONNET_EXPORT
#define KDECORE_EXPORT_DEPRECATED KDE_DEPRECATED
#define KDEUI_EXPORT_DEPRECATED KDE_DEPRECATED
#define KIO_EXPORT_DEPRECATED KDE_DEPRECATED
#define KPARTS_EXPORT_DEPRECATED KDE_DEPRECATED

#endif
