/* qt4compat: kcursor.cpp */
#include <kcursor.h>

static int s_hideDelay = 5000;

void KCursor::setAutoHideCursor(QWidget *, bool, bool)
{
}

void KCursor::setHideCursorDelay(int ms)
{
    s_hideDelay = ms;
}

int KCursor::hideCursorDelay()
{
    return s_hideDelay;
}

void KCursor::autoHideEventFilter(QObject *, QEvent *)
{
}
