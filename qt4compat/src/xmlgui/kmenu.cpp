/* qt4compat: kmenu.cpp */
#include <kmenu.h>

#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidgetAction>

class KMenu::KMenuPrivate
{
public:
    KMenuPrivate() : ctxMenu(0), buttons(Qt::NoButton), modifiers(Qt::NoModifier) {}
    QMenu *ctxMenu;
    Qt::MouseButtons buttons;
    Qt::KeyboardModifiers modifiers;
};

KMenu::KMenu(QWidget *parent)
    : QMenu(parent), d(new KMenuPrivate)
{
}

KMenu::KMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent), d(new KMenuPrivate)
{
}

KMenu::~KMenu()
{
    delete d;
}

QAction *KMenu::addTitle(const QString &text, QAction *before)
{
    return addTitle(QIcon(), text, before);
}

QAction *KMenu::addTitle(const QIcon &icon, const QString &text, QAction *before)
{
    QWidgetAction *action = new QWidgetAction(this);
    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(w);
    layout->setContentsMargins(6, 3, 6, 3);
    layout->setSpacing(4);
    if (!icon.isNull()) {
        QLabel *iconLabel = new QLabel(w);
        iconLabel->setPixmap(icon.pixmap(16, 16));
        layout->addWidget(iconLabel);
    }
    QLabel *label = new QLabel(text, w);
    QFont f = label->font();
    f.setBold(true);
    label->setFont(f);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, 1);
    action->setDefaultWidget(w);
    action->setText(text);
    action->setEnabled(false);
    insertAction(before, action);
    return action;
}

void KMenu::setKeyboardShortcutsEnabled(bool)
{
}

void KMenu::setKeyboardShortcutsExecute(bool)
{
}

QMenu *KMenu::contextMenu()
{
    if (!d->ctxMenu)
        d->ctxMenu = new QMenu(this);
    return d->ctxMenu;
}

const QMenu *KMenu::contextMenu() const
{
    return const_cast<KMenu *>(this)->contextMenu();
}

void KMenu::hideContextMenu()
{
    if (d->ctxMenu)
        d->ctxMenu->hide();
}

KMenu *KMenu::contextMenuFocus()
{
    return 0;
}

QAction *KMenu::contextMenuFocusAction()
{
    return 0;
}

Qt::MouseButtons KMenu::mouseButtons() const
{
    return d->buttons;
}

Qt::KeyboardModifiers KMenu::keyboardModifiers() const
{
    return d->modifiers;
}

void KMenu::mousePressEvent(QMouseEvent *e)
{
    d->buttons = e->buttons();
    d->modifiers = e->modifiers();
    QMenu::mousePressEvent(e);
}

void KMenu::mouseReleaseEvent(QMouseEvent *e)
{
    d->buttons = e->buttons();
    d->modifiers = e->modifiers();
    QMenu::mouseReleaseEvent(e);
}

void KMenu::keyPressEvent(QKeyEvent *e)
{
    d->modifiers = e->modifiers();
    QMenu::keyPressEvent(e);
}
