/* qt4compat: kdebug.h - kDebug()/kWarning()/kError()/kFatal() on top of QDebug.
 * kDebug output is silent unless the environment variable KDE_DEBUG or
 * KATE_DEBUG is set; warnings and errors are always printed. */
#ifndef KDECOMPAT_KDEBUG_H
#define KDECOMPAT_KDEBUG_H

#include <QtCore/QDebug>
#include <kdemacros.h>

#ifndef KDE_DEFAULT_DEBUG_AREA
#define KDE_DEFAULT_DEBUG_AREA 0
#endif

KDECORE_EXPORT QDebug kDebugStream(QtMsgType level, int area, const char *file = 0, int line = -1, const char *funcinfo = 0);
KDECORE_EXPORT QDebug kDebugDevNull();
KDECORE_EXPORT QString kBacktrace(int levels = -1);
KDECORE_EXPORT void kClearDebugConfig();

static inline QDebug kDebug(int area = KDE_DEFAULT_DEBUG_AREA)
{ return kDebugStream(QtDebugMsg, area); }
static inline QDebug kDebug(bool cond, int area = KDE_DEFAULT_DEBUG_AREA)
{ return cond ? kDebugStream(QtDebugMsg, area) : kDebugDevNull(); }
static inline QDebug kWarning(int area = KDE_DEFAULT_DEBUG_AREA)
{ return kDebugStream(QtWarningMsg, area); }
static inline QDebug kWarning(bool cond, int area = KDE_DEFAULT_DEBUG_AREA)
{ return cond ? kDebugStream(QtWarningMsg, area) : kDebugDevNull(); }
static inline QDebug kError(int area = KDE_DEFAULT_DEBUG_AREA)
{ return kDebugStream(QtCriticalMsg, area); }
static inline QDebug kError(bool cond, int area = KDE_DEFAULT_DEBUG_AREA)
{ return cond ? kDebugStream(QtCriticalMsg, area) : kDebugDevNull(); }
static inline QDebug kFatal(int area = KDE_DEFAULT_DEBUG_AREA)
{ return kDebugStream(QtFatalMsg, area); }
static inline QDebug kFatal(bool cond, int area = KDE_DEFAULT_DEBUG_AREA)
{ return cond ? kDebugStream(QtFatalMsg, area) : kDebugDevNull(); }

#define k_funcinfo ""
#define k_lineinfo "[" __FILE__ ":" QT_STRINGIFY(__LINE__) "] "
#define kdDebug kDebug
#define kdWarning kWarning
#define kdError kError
#define kdFatal kFatal

class KDECORE_EXPORT KDebug
{
public:
    static int registerArea(const QByteArray &areaName, bool enabled = true);
    static bool hasNullOutput(QtMsgType type, bool condition, int area, bool enableByDefault);
    static bool hasNullOutputQtDebugMsg(int area = KDE_DEFAULT_DEBUG_AREA)
    { return hasNullOutput(QtDebugMsg, true, area, true); }
};

#endif
