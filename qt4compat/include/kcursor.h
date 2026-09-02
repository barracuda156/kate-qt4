/* qt4compat: kcursor.h - auto hiding of the mouse cursor is not implemented */
#ifndef KDECOMPAT_KCURSOR_H
#define KDECOMPAT_KCURSOR_H

#include <QtCore/QObject>
#include <kdemacros.h>

class QWidget;
class QEvent;

class KDEUI_EXPORT KCursor
{
public:
    static void setAutoHideCursor(QWidget *w, bool enable, bool customEventFilter = false);
    static void setHideCursorDelay(int ms);
    static int hideCursorDelay();
    static void autoHideEventFilter(QObject *, QEvent *);
};

#endif
