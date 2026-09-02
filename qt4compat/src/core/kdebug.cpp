/* qt4compat: kdebug.cpp */
#include <kdebug.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QIODevice>

namespace {

class KNullDevice : public QIODevice
{
public:
    KNullDevice() { open(QIODevice::WriteOnly); }
protected:
    qint64 readData(char *, qint64) { return -1; }
    qint64 writeData(const char *, qint64 len) { return len; }
};

static KNullDevice *nullDevice()
{
    static KNullDevice dev;
    return &dev;
}

static bool debugEnabled()
{
    static int enabled = -1;
    if (enabled < 0)
        enabled = (!qgetenv("KDE_DEBUG").isEmpty() || !qgetenv("KATE_DEBUG").isEmpty()) ? 1 : 0;
    return enabled == 1;
}

} // namespace

QDebug kDebugDevNull()
{
    return QDebug(nullDevice());
}

QDebug kDebugStream(QtMsgType level, int area, const char *, int, const char *)
{
    if (level == QtDebugMsg && !debugEnabled())
        return kDebugDevNull();

    QDebug s(level);
    QByteArray app = QCoreApplication::instance() ? QCoreApplication::applicationName().toLocal8Bit() : QByteArray("kate");
    s.nospace() << app.constData() << "(" << area << "):";
    s.space();
    return s;
}

QString kBacktrace(int)
{
    return QString();
}

void kClearDebugConfig()
{
}

int KDebug::registerArea(const QByteArray &, bool)
{
    return 0;
}

bool KDebug::hasNullOutput(QtMsgType type, bool condition, int, bool)
{
    if (!condition)
        return true;
    if (type == QtDebugMsg)
        return !debugEnabled();
    return false;
}
