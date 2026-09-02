/* qt4compat: kactionmenu.cpp */
#include <kactionmenu.h>
#include <kmenu.h>

#include <QtGui/QToolBar>
#include <QtGui/QToolButton>

class KActionMenuPrivate
{
public:
    KActionMenuPrivate() : m_delayed(true), m_stickyMenu(true) {}
    bool m_delayed;
    bool m_stickyMenu;
};

KActionMenu::KActionMenu(QObject *parent)
    : KAction(parent), d(new KActionMenuPrivate)
{
    setShortcutConfigurable(false);
}

KActionMenu::KActionMenu(const QString &text, QObject *parent)
    : KAction(text, parent), d(new KActionMenuPrivate)
{
    setShortcutConfigurable(false);
}

KActionMenu::KActionMenu(const KIcon &icon, const QString &text, QObject *parent)
    : KAction(icon, text, parent), d(new KActionMenuPrivate)
{
    setShortcutConfigurable(false);
}

KActionMenu::~KActionMenu()
{
    delete d;
    delete menu();
}

QWidget *KActionMenu::createWidget(QWidget *parent)
{
    QToolBar *toolBar = qobject_cast<QToolBar *>(parent);
    if (!toolBar)
        return 0;
    QToolButton *button = new QToolButton(toolBar);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setIconSize(toolBar->iconSize());
    button->setToolButtonStyle(toolBar->toolButtonStyle());
    QObject::connect(toolBar, SIGNAL(iconSizeChanged(QSize)), button, SLOT(setIconSize(QSize)));
    QObject::connect(toolBar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
                     button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
    button->setDefaultAction(this);
    QObject::connect(button, SIGNAL(triggered(QAction*)), toolBar, SIGNAL(actionTriggered(QAction*)));
    if (delayed())
        button->setPopupMode(stickyMenu() ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup);
    else
        button->setPopupMode(QToolButton::InstantPopup);
    return button;
}

void KActionMenu::remove(KAction *cmd)
{
    if (cmd)
        menu()->removeAction(cmd);
}

void KActionMenu::addAction(QAction *action)
{
    menu()->addAction(action);
}

QAction *KActionMenu::addSeparator()
{
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    return separator;
}

QAction *KActionMenu::insertSeparator(QAction *before)
{
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    insertAction(before, separator);
    return separator;
}

void KActionMenu::insertAction(QAction *before, QAction *action)
{
    menu()->insertAction(before, action);
}

void KActionMenu::removeAction(QAction *action)
{
    menu()->removeAction(action);
}

bool KActionMenu::delayed() const
{
    return d->m_delayed;
}

void KActionMenu::setDelayed(bool delayed)
{
    d->m_delayed = delayed;
}

bool KActionMenu::stickyMenu() const
{
    return d->m_stickyMenu;
}

void KActionMenu::setStickyMenu(bool sticky)
{
    d->m_stickyMenu = sticky;
}

KMenu *KActionMenu::menu()
{
    if (!QAction::menu())
        setMenu(new KMenu());
    return qobject_cast<KMenu *>(QAction::menu());
}

void KActionMenu::setMenu(KMenu *menu)
{
    QAction::setMenu(menu);
}
