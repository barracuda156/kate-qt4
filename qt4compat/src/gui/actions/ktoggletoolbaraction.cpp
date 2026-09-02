/* qt4compat: ktoggletoolbaraction.cpp */
#include <ktoggletoolbaraction.h>

#include <kmainwindow.h>
#include <ktoolbar.h>

#include <QtCore/QEvent>
#include <QtCore/QPointer>

class KToggleToolBarAction::Private
{
public:
    Private() : beingToggled(false) {}
    QByteArray toolBarName;
    QPointer<KToolBar> toolBar;
    bool beingToggled;
};

KToggleToolBarAction::KToggleToolBarAction(const char *toolBarName, const QString &text, QObject *parent)
    : KToggleAction(text, parent), d(new Private)
{
    d->toolBarName = toolBarName;
}

KToggleToolBarAction::KToggleToolBarAction(KToolBar *toolBar, const QString &text, QObject *parent)
    : KToggleAction(text, parent), d(new Private)
{
    d->toolBar = toolBar;
    if (d->toolBar) {
        d->toolBar->installEventFilter(this);
        setChecked(d->toolBar->isVisible());
    }
}

KToggleToolBarAction::~KToggleToolBarAction()
{
    delete d;
}

KToolBar *KToggleToolBarAction::toolBar()
{
    if (!d->toolBar && !d->toolBarName.isEmpty()) {
        const QList<KMainWindow *> windows = KMainWindow::memberList();
        for (int i = 0; i < windows.count() && !d->toolBar; ++i) {
            KToolBar *tb = windows.at(i)->findChild<KToolBar *>(QString::fromLatin1(d->toolBarName.constData()));
            if (tb) {
                d->toolBar = tb;
                d->toolBar->installEventFilter(this);
                setChecked(d->toolBar->isVisible());
            }
        }
    }
    return d->toolBar;
}

bool KToggleToolBarAction::eventFilter(QObject *watched, QEvent *event)
{
    if (d->beingToggled)
        return false;
    if (watched == d->toolBar) {
        if (event->type() == QEvent::Hide || event->type() == QEvent::Show) {
            if (isChecked() != d->toolBar->isVisible()) {
                d->beingToggled = true;
                setChecked(d->toolBar->isVisible());
                d->beingToggled = false;
            }
        }
    }
    return KToggleAction::eventFilter(watched, event);
}

void KToggleToolBarAction::slotToggled(bool checked)
{
    if (!d->beingToggled && toolBar() && checked != d->toolBar->isVisible()) {
        d->beingToggled = true;
        d->toolBar->setVisible(checked);
        d->beingToggled = false;
        KMainWindow *mw = d->toolBar->mainWindow();
        if (mw)
            mw->setSettingsDirty();
    }
    KToggleAction::slotToggled(checked);
}
