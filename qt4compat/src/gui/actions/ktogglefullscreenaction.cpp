/* qt4compat: ktogglefullscreenaction.cpp */
#include <ktogglefullscreenaction.h>

#include <kguiitem.h>
#include <kicon.h>
#include <klocale.h>
#include <kstandardshortcut.h>

#include <QtCore/QEvent>
#include <QtGui/QWidget>

class KToggleFullScreenAction::Private
{
public:
    Private() : window(0) {}
    QWidget *window;
};

KToggleFullScreenAction::KToggleFullScreenAction(QObject *parent)
    : KToggleAction(parent), d(new Private)
{
    init();
}

KToggleFullScreenAction::KToggleFullScreenAction(QWidget *window, QObject *parent)
    : KToggleAction(parent), d(new Private)
{
    init();
    setWindow(window);
}

KToggleFullScreenAction::~KToggleFullScreenAction()
{
    delete d;
}

void KToggleFullScreenAction::init()
{
    setText(i18n("F&ull Screen Mode"));
    setIcon(KIcon(QLatin1String("view-fullscreen")));
    setShortcut(KStandardShortcut::fullScreen());
    setCheckedState(KGuiItem(i18n("Exit F&ull Screen Mode"), QLatin1String("view-restore")));
}

void KToggleFullScreenAction::setWindow(QWidget *window)
{
    if (d->window)
        d->window->removeEventFilter(this);
    d->window = window;
    if (d->window)
        d->window->installEventFilter(this);
}

void KToggleFullScreenAction::slotToggled(bool checked)
{
    if (d->window)
        setFullScreen(d->window, checked);
    KToggleAction::slotToggled(checked);
}

bool KToggleFullScreenAction::eventFilter(QObject *object, QEvent *event)
{
    if (object == d->window && event->type() == QEvent::WindowStateChange) {
        if (d->window->isFullScreen() != isChecked())
            activate(QAction::Trigger);
    }
    return KToggleAction::eventFilter(object, event);
}

void KToggleFullScreenAction::setFullScreen(QWidget *window, bool set)
{
    if (!window)
        return;
    if (set)
        window->setWindowState(window->windowState() | Qt::WindowFullScreen);
    else
        window->setWindowState(window->windowState() & ~Qt::WindowFullScreen);
}
