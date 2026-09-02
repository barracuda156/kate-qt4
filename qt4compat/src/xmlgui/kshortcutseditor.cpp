/* qt4compat: kshortcutseditor.cpp */
#include <kshortcutseditor.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kcomponentdata.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kkeysequencewidget.h>
#include <klocale.h>
#include <kshortcut.h>

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

namespace {

struct PendingShortcut
{
    KShortcut active;
    KShortcut original;
};

static QString actionLabel(const QAction *action)
{
    QString text = action->text();
    text.remove(QLatin1Char('&'));
    return text;
}

}

class KShortcutsEditorPrivate
{
public:
    KShortcutsEditorPrivate() : tree(0), filter(0), modified(false) {}
    QTreeWidget *tree;
    QLineEdit *filter;
    QList<KActionCollection *> collections;
    QMap<QTreeWidgetItem *, KAction *> actions;
    QMap<KAction *, PendingShortcut> pending;
    bool modified;
    KShortcutsEditor::ActionTypes actionTypes;
    KShortcutsEditor::LetterShortcuts letterShortcuts;
};

KShortcutsEditor::KShortcutsEditor(KActionCollection *collection, QWidget *parent, ActionTypes actionTypes, LetterShortcuts allowLetterShortcuts)
    : QWidget(parent), d(new KShortcutsEditorPrivate)
{
    d->actionTypes = actionTypes;
    d->letterShortcuts = allowLetterShortcuts;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    d->tree = new QTreeWidget(this);
    d->tree->setColumnCount(3);
    d->tree->setHeaderLabels(QStringList() << i18n("Action") << i18n("Shortcut") << i18n("Alternate"));
    d->tree->setRootIsDecorated(true);
    d->tree->setAllColumnsShowFocus(true);
    d->tree->header()->setResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(d->tree);
    QLabel *hint = new QLabel(i18n("Double-click a shortcut column to change it. Escape cancels."), this);
    hint->setWordWrap(true);
    layout->addWidget(hint);
    connect(d->tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
    if (collection)
        addCollection(collection);
}

KShortcutsEditor::KShortcutsEditor(QWidget *parent, ActionTypes actionTypes, LetterShortcuts allowLetterShortcuts)
    : QWidget(parent), d(new KShortcutsEditorPrivate)
{
    d->actionTypes = actionTypes;
    d->letterShortcuts = allowLetterShortcuts;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    d->tree = new QTreeWidget(this);
    d->tree->setColumnCount(3);
    d->tree->setHeaderLabels(QStringList() << i18n("Action") << i18n("Shortcut") << i18n("Alternate"));
    d->tree->setRootIsDecorated(true);
    d->tree->setAllColumnsShowFocus(true);
    d->tree->header()->setResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(d->tree);
    QLabel *hint = new QLabel(i18n("Double-click a shortcut column to change it. Escape cancels."), this);
    hint->setWordWrap(true);
    layout->addWidget(hint);
    connect(d->tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(itemActivated(QTreeWidgetItem*,int)));
}

KShortcutsEditor::~KShortcutsEditor()
{
    delete d;
}

bool KShortcutsEditor::isModified() const
{
    return d->modified;
}

void KShortcutsEditor::clearCollections()
{
    d->tree->clear();
    d->collections.clear();
    d->actions.clear();
    d->pending.clear();
    d->modified = false;
}

static void fillItem(QTreeWidgetItem *item, const KShortcut &cut)
{
    item->setText(1, cut.primary().toString(QKeySequence::NativeText));
    item->setText(2, cut.alternate().toString(QKeySequence::NativeText));
}

void KShortcutsEditor::addCollection(KActionCollection *collection, const QString &title)
{
    if (!collection || d->collections.contains(collection))
        return;
    d->collections.append(collection);

    QString displayTitle = title;
    if (displayTitle.isEmpty()) {
        displayTitle = collection->componentData().aboutData() ? collection->componentData().aboutData()->programName() : QString();
        if (displayTitle.isEmpty())
            displayTitle = collection->componentData().componentName();
        if (displayTitle.isEmpty())
            displayTitle = i18n("Shortcuts");
    }
    QTreeWidgetItem *top = new QTreeWidgetItem(d->tree, QStringList(displayTitle));
    QFont f = top->font(0);
    f.setBold(true);
    top->setFont(0, f);
    top->setFlags(Qt::ItemIsEnabled);

    QList<QAction *> actions = collection->actions();
    QMap<QString, KAction *> sorted;
    foreach (QAction *action, actions) {
        KAction *kaction = qobject_cast<KAction *>(action);
        if (!kaction || !kaction->isShortcutConfigurable() || action->isSeparator() || action->text().isEmpty())
            continue;
        sorted.insertMulti(actionLabel(action).toLower(), kaction);
    }
    for (QMap<QString, KAction *>::ConstIterator it = sorted.constBegin(); it != sorted.constEnd(); ++it) {
        KAction *kaction = it.value();
        QTreeWidgetItem *item = new QTreeWidgetItem(top, QStringList(actionLabel(kaction)));
        item->setIcon(0, kaction->icon());
        item->setToolTip(0, kaction->whatsThis().isEmpty() ? kaction->toolTip() : kaction->whatsThis());
        PendingShortcut p;
        p.active = kaction->shortcut(KAction::ActiveShortcut);
        p.original = p.active;
        d->pending.insert(kaction, p);
        d->actions.insert(item, kaction);
        fillItem(item, p.active);
    }
    d->tree->expandAll();
    d->tree->resizeColumnToContents(1);
    d->tree->resizeColumnToContents(2);
}

void KShortcutsEditor::undoChanges()
{
    for (QMap<QTreeWidgetItem *, KAction *>::ConstIterator it = d->actions.constBegin(); it != d->actions.constEnd(); ++it) {
        PendingShortcut &p = d->pending[it.value()];
        p.active = p.original;
        fillItem(it.key(), p.active);
    }
    d->modified = false;
}

void KShortcutsEditor::save()
{
    commit();
    foreach (KActionCollection *collection, d->collections)
        collection->writeSettings();
}

void KShortcutsEditor::commit()
{
    for (QMap<KAction *, PendingShortcut>::Iterator it = d->pending.begin(); it != d->pending.end(); ++it) {
        if (it.value().active != it.key()->shortcut(KAction::ActiveShortcut))
            it.key()->setShortcut(it.value().active, KAction::ActiveShortcut);
        it.value().original = it.value().active;
    }
    d->modified = false;
}

void KShortcutsEditor::exportConfiguration(KConfig *config) const
{
    if (!config)
        return;
    KConfigGroup group(config, "Shortcuts");
    foreach (KActionCollection *collection, d->collections)
        collection->writeSettings(&group, true);
}

void KShortcutsEditor::importConfiguration(KConfig *config)
{
    if (!config)
        return;
    KConfigGroup group(config, "Shortcuts");
    foreach (KActionCollection *collection, d->collections)
        collection->readSettings(&group);
    for (QMap<QTreeWidgetItem *, KAction *>::ConstIterator it = d->actions.constBegin(); it != d->actions.constEnd(); ++it) {
        PendingShortcut &p = d->pending[it.value()];
        p.active = it.value()->shortcut(KAction::ActiveShortcut);
        fillItem(it.key(), p.active);
    }
    d->modified = true;
}

void KShortcutsEditor::allDefault()
{
    for (QMap<QTreeWidgetItem *, KAction *>::ConstIterator it = d->actions.constBegin(); it != d->actions.constEnd(); ++it) {
        PendingShortcut &p = d->pending[it.value()];
        p.active = it.value()->shortcut(KAction::DefaultShortcut);
        fillItem(it.key(), p.active);
    }
    d->modified = true;
    emit keyChange();
}

void KShortcutsEditor::printShortcuts() const
{
}

void KShortcutsEditor::itemActivated(QTreeWidgetItem *item, int column)
{
    if (!item || !d->actions.contains(item))
        return;
    if (column < 1)
        column = 1;
    KAction *action = d->actions.value(item);
    PendingShortcut &p = d->pending[action];

    QDialog dlg(this);
    dlg.setWindowTitle(column == 1 ? i18n("Shortcut for '%1'", actionLabel(action)) : i18n("Alternate shortcut for '%1'", actionLabel(action)));
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(new QLabel(i18n("Press the new shortcut, or clear it:"), &dlg));
    KKeySequenceWidget *keyWidget = new KKeySequenceWidget(&dlg);
    keyWidget->setModifierlessAllowed(d->letterShortcuts == LetterShortcutsAllowed);
    keyWidget->setKeySequence(column == 1 ? p.active.primary() : p.active.alternate());
    layout->addWidget(keyWidget);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dlg);
    connect(buttons, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &dlg, SLOT(reject()));
    layout->addWidget(buttons);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QKeySequence seq = keyWidget->keySequence();
    if (column == 1)
        p.active.setPrimary(seq);
    else
        p.active.setAlternate(seq);
    /* avoid the same shortcut being used twice within the edited collections */
    for (QMap<KAction *, PendingShortcut>::Iterator it = d->pending.begin(); it != d->pending.end(); ++it) {
        if (it.key() == action || seq.isEmpty())
            continue;
        if (it.value().active.contains(seq)) {
            it.value().active.remove(seq, KShortcut::KeepEmpty);
            for (QMap<QTreeWidgetItem *, KAction *>::ConstIterator ai = d->actions.constBegin(); ai != d->actions.constEnd(); ++ai)
                if (ai.value() == it.key())
                    fillItem(ai.key(), it.value().active);
        }
    }
    fillItem(item, p.active);
    d->modified = true;
    emit keyChange();
}
