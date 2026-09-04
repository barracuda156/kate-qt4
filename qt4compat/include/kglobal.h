/* qt4compat: kglobal.h */
#ifndef KDECOMPAT_KGLOBAL_H
#define KDECOMPAT_KGLOBAL_H

#include <QtCore/QAtomicPointer>
#include <QtCore/QString>
#include <kdemacros.h>
#include <ksharedptr.h>

class KAboutData;
class KCharsets;
class KComponentData;
class KConfig;
class KLocale;
class KSharedConfig;
class KStandardDirs;
typedef KSharedPtr<KSharedConfig> KSharedConfigPtr;

/* K_GLOBAL_STATIC, as in kdelibs (single-threaded use is enough here) */
typedef void (*KdeCleanUpFunction)();
class KDECORE_EXPORT KCleanUpGlobalStatic
{
public:
    KdeCleanUpFunction func;
    inline ~KCleanUpGlobalStatic() { func(); }
};

#define K_GLOBAL_STATIC_STRUCT_NAME(NAME)

#define K_GLOBAL_STATIC(TYPE, NAME) K_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ())

#define K_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                            \
static QBasicAtomicPointer<TYPE > _k_static_##NAME = Q_BASIC_ATOMIC_INITIALIZER(0); \
static bool _k_static_##NAME##_destroyed;                                      \
static struct K_GLOBAL_STATIC_STRUCT_NAME(NAME)                                \
{                                                                              \
    inline bool isDestroyed() const                                            \
    {                                                                          \
        return _k_static_##NAME##_destroyed;                                   \
    }                                                                          \
    inline bool exists() const                                                 \
    {                                                                          \
        return _k_static_##NAME != 0;                                          \
    }                                                                          \
    inline operator TYPE*()                                                    \
    {                                                                          \
        return operator->();                                                   \
    }                                                                          \
    inline TYPE *operator->()                                                  \
    {                                                                          \
        if (!_k_static_##NAME) {                                               \
            if (isDestroyed()) {                                               \
                qFatal("Fatal Error: Accessed global static '%s *%s()' after destruction. " \
                       "Defined at %s:%d", #TYPE, #NAME, __FILE__, __LINE__);  \
            }                                                                  \
            TYPE *x = new TYPE ARGS;                                           \
            if (!_k_static_##NAME.testAndSetOrdered(0, x)                      \
                && _k_static_##NAME != x ) {                                   \
                delete x;                                                      \
            } else {                                                           \
                static KCleanUpGlobalStatic cleanUpObject = { destroy };       \
            }                                                                  \
        }                                                                      \
        return _k_static_##NAME;                                               \
    }                                                                          \
    inline TYPE &operator*()                                                   \
    {                                                                          \
        return *operator->();                                                  \
    }                                                                          \
    static void destroy()                                                      \
    {                                                                          \
        _k_static_##NAME##_destroyed = true;                                   \
        TYPE *x = _k_static_##NAME;                                            \
        _k_static_##NAME = 0;                                                  \
        delete x;                                                              \
    }                                                                          \
} NAME;

template<typename T>
inline const T &kClamp(const T &x, const T &low, const T &high)
{
    if (x < low)
        return low;
    else if (high < x)
        return high;
    else
        return x;
}

namespace KGlobal
{
    enum CopyCatalogs { DoCopyCatalogs, DontCopyCatalogs };

    KDECORE_EXPORT const KComponentData &mainComponent();
    KDECORE_EXPORT bool hasMainComponent();
    KDECORE_EXPORT KComponentData activeComponent();
    KDECORE_EXPORT void setActiveComponent(const KComponentData &d);

    KDECORE_EXPORT KSharedConfigPtr config();
    KDECORE_EXPORT KStandardDirs *dirs();
    KDECORE_EXPORT KLocale *locale();
    KDECORE_EXPORT bool hasLocale();
    KDECORE_EXPORT void setLocale(KLocale *locale, CopyCatalogs copy = DoCopyCatalogs);
    KDECORE_EXPORT KCharsets *charsets();

    KDECORE_EXPORT QString caption();
    KDECORE_EXPORT const QString &staticQString(const char *str);
    KDECORE_EXPORT const QString &staticQString(const QString &str);

    KDECORE_EXPORT void ref();
    KDECORE_EXPORT void deref();
    KDECORE_EXPORT void setAllowQuit(bool allowQuit);

    KDECORE_EXPORT mode_t umask();
}

#ifdef KDE_SUPPORT
#define kapp KApplication::kApplication()
#endif

#endif
