/* qt4compat: kselectaction.cpp */
#include "kselectaction_p.h"
#include <kicon.h>
#include <kmenu.h>

#include <QtCore/QEvent>
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

static QString dropAmpersands(const QString &text)
{
    QString result;
    result.reserve(text.length());
    for (int i = 0; i < text.length(); ++i) {
        if (text.at(i) == QLatin1Char('&')) {
            if (i + 1 < text.length() && text.at(i + 1) == QLatin1Char('&')) {
                result += QLatin1Char('&');
                ++i;
            }
            continue;
        }
        result += text.at(i);
    }
    return result;
}

KSelectAction::KSelectAction(QObject *parent)
    : KAction(parent), d_ptr(new KSelectActionPrivate)
{
    init();
}

KSelectAction::KSelectAction(const QString &text, QObject *parent)
    : KAction(text, parent), d_ptr(new KSelectActionPrivate)
{
    init();
}

KSelectAction::KSelectAction(const KIcon &icon, const QString &text, QObject *parent)
    : KAction(icon, text, parent), d_ptr(new KSelectActionPrivate)
{
    init();
}

KSelectAction::KSelectAction(KSelectActionPrivate &dd, QObject *parent)
    : KAction(parent), d_ptr(&dd)
{
    init();
}

KSelectAction::~KSelectAction()
{
    delete menu();
    delete d_ptr;
}

void KSelectAction::init()
{
    Q_D(KSelectAction);
    d->m_actionGroup = new QActionGroup(this);
    d->m_actionGroup->setExclusive(true);
    connect(d->m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
    connect(this, SIGNAL(changed()), this, SLOT(syncWidgets()));
    setMenu(new KMenu());
    setEnabled(false);
}

QActionGroup *KSelectAction::selectableActionGroup() const
{
    Q_D(const KSelectAction);
    return d->m_actionGroup;
}

QList<QAction *> KSelectAction::actions() const
{
    return selectableActionGroup()->actions();
}

QAction *KSelectAction::currentAction() const
{
    return selectableActionGroup()->checkedAction();
}

int KSelectAction::currentItem() const
{
    return selectableActionGroup()->actions().indexOf(currentAction());
}

QString KSelectAction::currentText() const
{
    if (QAction *a = currentAction())
        return dropAmpersands(a->text());
    return QString();
}

QAction *KSelectAction::action(int index) const
{
    return actions().value(index);
}

QAction *KSelectAction::action(const QString &text, Qt::CaseSensitivity cs) const
{
    const QString compare = dropAmpersands(text);
    foreach (QAction *action, actions()) {
        if (dropAmpersands(action->text()).compare(compare, cs) == 0)
            return action;
    }
    return 0;
}

bool KSelectAction::setCurrentAction(QAction *action)
{
    Q_D(KSelectAction);
    if (action) {
        if (actions().contains(action)) {
            if (action->isVisible() && action->isEnabled() && action->isCheckable()) {
                action->setChecked(true);
                foreach (QComboBox *box, d->m_comboBoxes)
                    box->setCurrentIndex(actions().indexOf(action));
                return true;
            }
        }
    } else if (QAction *cur = currentAction()) {
        cur->setChecked(false);
    }
    return false;
}

bool KSelectAction::setCurrentItem(int index)
{
    return setCurrentAction(action(index));
}

bool KSelectAction::setCurrentAction(const QString &text, Qt::CaseSensitivity cs)
{
    return setCurrentAction(action(text, cs));
}

void KSelectAction::addAction(QAction *action)
{
    Q_D(KSelectAction);
    action->setActionGroup(selectableActionGroup());
    setEnabled(true);
    foreach (QToolButton *button, d->m_buttons)
        button->setEnabled(true);
    foreach (QComboBox *box, d->m_comboBoxes) {
        box->setEnabled(true);
        box->addItem(action->icon(), dropAmpersands(action->text()));
    }
    menu()->addAction(action);
}

KAction *KSelectAction::addAction(const QString &text)
{
    Q_D(KSelectAction);
    KAction *action = new KAction(selectableActionGroup());
    action->setText(d->makeMenuText(text));
    action->setCheckable(true);
    action->setShortcutConfigurable(false);
    if (!d->m_menuAccelsEnabled) {
        action->setText(text);
        action->setShortcut(QKeySequence());
    }
    addAction(action);
    return action;
}

KAction *KSelectAction::addAction(const KIcon &icon, const QString &text)
{
    KAction *action = addAction(text);
    action->setIcon(icon);
    return action;
}

QAction *KSelectAction::removeAction(QAction *action)
{
    Q_D(KSelectAction);
    const int index = actions().indexOf(action);
    action->setActionGroup(0);
    menu()->removeAction(action);
    foreach (QComboBox *box, d->m_comboBoxes) {
        if (index >= 0 && index < box->count())
            box->removeItem(index);
    }
    return action;
}

void KSelectAction::setItems(const QStringList &lst)
{
    Q_D(KSelectAction);
    clear();
    foreach (const QString &string, lst) {
        if (!string.isEmpty()) {
            addAction(string);
        } else {
            QAction *action = new QAction(this);
            action->setSeparator(true);
            addAction(action);
        }
    }
    setEnabled(lst.count() > 0 || d->m_edit);
}

QStringList KSelectAction::items() const
{
    QStringList ret;
    foreach (QAction *action, selectableActionGroup()->actions())
        ret << dropAmpersands(action->text());
    return ret;
}

bool KSelectAction::isEditable() const
{
    Q_D(const KSelectAction);
    return d->m_edit;
}

void KSelectAction::setEditable(bool edit)
{
    Q_D(KSelectAction);
    d->m_edit = edit;
    foreach (QComboBox *box, d->m_comboBoxes)
        box->setEditable(edit);
}

int KSelectAction::comboWidth() const
{
    Q_D(const KSelectAction);
    return d->m_comboWidth;
}

void KSelectAction::setComboWidth(int width)
{
    Q_D(KSelectAction);
    if (width < 0)
        return;
    d->m_comboWidth = width;
    foreach (QComboBox *box, d->m_comboBoxes) {
        box->setMaximumWidth(width);
        box->setMinimumWidth(width);
    }
}

void KSelectAction::setMaxComboViewCount(int n)
{
    Q_D(KSelectAction);
    d->m_maxComboViewCount = n;
    foreach (QComboBox *box, d->m_comboBoxes) {
        if (n != -1)
            box->setMaxVisibleItems(n);
    }
}

void KSelectAction::clear()
{
    Q_D(KSelectAction);
    const QList<QAction *> list = actions();
    for (int i = 0; i < list.count(); ++i) {
        QAction *a = list.at(i);
        a->setActionGroup(0);
        menu()->removeAction(a);
        a->deleteLater();
    }
    foreach (QComboBox *box, d->m_comboBoxes)
        box->clear();
}

void KSelectAction::removeAllActions()
{
    while (!selectableActionGroup()->actions().isEmpty())
        removeAction(selectableActionGroup()->actions().first());
}

void KSelectAction::setMenuAccelsEnabled(bool b)
{
    Q_D(KSelectAction);
    d->m_menuAccelsEnabled = b;
}

bool KSelectAction::menuAccelsEnabled() const
{
    Q_D(const KSelectAction);
    return d->m_menuAccelsEnabled;
}

void KSelectAction::changeItem(int index, const QString &text)
{
    Q_D(KSelectAction);
    if (index < 0 || index >= actions().count())
        return;
    actions().at(index)->setText(d->makeMenuText(text));
    foreach (QComboBox *box, d->m_comboBoxes)
        box->setItemText(index, dropAmpersands(text));
}

KSelectAction::ToolBarMode KSelectAction::toolBarMode() const
{
    Q_D(const KSelectAction);
    return d->m_toolBarMode;
}

void KSelectAction::setToolBarMode(ToolBarMode mode)
{
    Q_D(KSelectAction);
    d->m_toolBarMode = mode;
}

QToolButton::ToolButtonPopupMode KSelectAction::toolButtonPopupMode() const
{
    Q_D(const KSelectAction);
    return d->m_toolButtonPopupMode;
}

void KSelectAction::setToolButtonPopupMode(QToolButton::ToolButtonPopupMode mode)
{
    Q_D(KSelectAction);
    d->m_toolButtonPopupMode = mode;
    foreach (QToolButton *button, d->m_buttons)
        button->setPopupMode(mode);
}

void KSelectAction::actionTriggered(QAction *action)
{
    Q_D(KSelectAction);
    const QString text = dropAmpersands(action->text());
    const int index = selectableActionGroup()->actions().indexOf(action);
    foreach (QComboBox *box, d->m_comboBoxes) {
        if (box->currentIndex() != index)
            box->setCurrentIndex(index);
    }
    emit triggered(action);
    emit triggered(index);
    emit triggered(text);
}

void KSelectAction::slotToggled(bool)
{
}

void KSelectAction::comboBoxActivated(int index)
{
    QAction *a = action(index);
    if (a)
        a->trigger();
}

void KSelectAction::comboBoxDeleted(QObject *object)
{
    Q_D(KSelectAction);
    d->m_comboBoxes.removeAll(static_cast<QComboBox *>(object));
}

void KSelectAction::toolButtonDeleted(QObject *object)
{
    Q_D(KSelectAction);
    d->m_buttons.removeAll(static_cast<QToolButton *>(object));
}

void KSelectAction::syncWidgets()
{
    Q_D(KSelectAction);
    foreach (QComboBox *box, d->m_comboBoxes) {
        box->setEnabled(isEnabled());
        box->setToolTip(toolTip());
    }
    foreach (QToolButton *button, d->m_buttons)
        button->setEnabled(isEnabled());
}

QWidget *KSelectAction::createWidget(QWidget *parent)
{
    Q_D(KSelectAction);
    if (qobject_cast<QMenu *>(parent))
        return 0;
    QToolBar *toolBar = qobject_cast<QToolBar *>(parent);
    if (!toolBar)
        return 0;

    switch (toolBarMode()) {
    case MenuMode: {
        QToolButton *button = new QToolButton(toolBar);
        button->setToolTip(toolTip());
        button->setWhatsThis(whatsThis());
        button->setStatusTip(statusTip());
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIconSize(toolBar->iconSize());
        button->setToolButtonStyle(toolBar->toolButtonStyle());
        connect(toolBar, SIGNAL(iconSizeChanged(QSize)), button, SLOT(setIconSize(QSize)));
        connect(toolBar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
                button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
        button->setDefaultAction(this);
        button->setPopupMode(toolButtonPopupMode());
        connect(button, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDeleted(QObject*)));
        d->m_buttons.append(button);
        return button;
    }
    case ComboBoxMode: {
        QComboBox *comboBox = new QComboBox(parent);
        comboBox->setEnabled(isEnabled());
        comboBox->setToolTip(toolTip());
        comboBox->setWhatsThis(whatsThis());
        comboBox->setStatusTip(statusTip());
        comboBox->setEditable(d->m_edit);
        if (d->m_comboWidth > 0) {
            comboBox->setMaximumWidth(d->m_comboWidth);
            comboBox->setMinimumWidth(d->m_comboWidth);
        }
        if (d->m_maxComboViewCount != -1)
            comboBox->setMaxVisibleItems(d->m_maxComboViewCount);
        foreach (QAction *action, actions())
            comboBox->addItem(action->icon(), dropAmpersands(action->text()));
        comboBox->setCurrentIndex(currentItem());
        connect(comboBox, SIGNAL(activated(int)), this, SLOT(comboBoxActivated(int)));
        connect(comboBox, SIGNAL(destroyed(QObject*)), this, SLOT(comboBoxDeleted(QObject*)));
        d->m_comboBoxes.append(comboBox);
        return comboBox;
    }
    }
    return 0;
}

void KSelectAction::deleteWidget(QWidget *widget)
{
    Q_D(KSelectAction);
    if (QToolButton *button = qobject_cast<QToolButton *>(widget))
        d->m_buttons.removeAll(button);
    else if (QComboBox *box = qobject_cast<QComboBox *>(widget))
        d->m_comboBoxes.removeAll(box);
    QWidgetAction::deleteWidget(widget);
}

bool KSelectAction::event(QEvent *event)
{
    return KAction::event(event);
}

bool KSelectAction::eventFilter(QObject *watched, QEvent *event)
{
    return KAction::eventFilter(watched, event);
}
