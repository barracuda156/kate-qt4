/* qt4compat: kshortcutsdialog.cpp */
#include <kshortcutsdialog.h>

#include <kactioncollection.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksharedconfig.h>

#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

class KShortcutsDialog::Private
{
public:
    Private() : editor(0), saveSettings(true) {}
    KShortcutsEditor *editor;
    QList<KActionCollection *> collections;
    bool saveSettings;
};

KShortcutsDialog::KShortcutsDialog(KShortcutsEditor::ActionTypes types, KShortcutsEditor::LetterShortcuts allowLetterShortcuts, QWidget *parent)
    : QDialog(parent), d(new Private)
{
    setWindowTitle(i18n("Configure Shortcuts"));
    setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    d->editor = new KShortcutsEditor(this, types, allowLetterShortcuts);
    layout->addWidget(d->editor);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(slotDefaults()));
    layout->addWidget(buttons);

    KConfigGroup group(KGlobal::config(), "KShortcutsDialog Settings");
    resize(group.readEntry("Dialog Size", sizeHint()));
}

KShortcutsDialog::~KShortcutsDialog()
{
    KConfigGroup group(KGlobal::config(), "KShortcutsDialog Settings");
    group.writeEntry("Dialog Size", size(), KConfigGroup::Persistent | KConfigGroup::Global);
    delete d;
}

void KShortcutsDialog::addCollection(KActionCollection *collection, const QString &title)
{
    d->editor->addCollection(collection, title);
    d->collections << collection;
}

QList<KActionCollection *> KShortcutsDialog::actionCollections() const
{
    return d->collections;
}

bool KShortcutsDialog::configure(bool saveSettings)
{
    d->saveSettings = saveSettings;
    const int retcode = exec();
    if (retcode == QDialog::Accepted) {
        if (saveSettings)
            d->editor->save();
        else
            d->editor->commit();
    } else {
        d->editor->undoChanges();
    }
    return retcode == QDialog::Accepted;
}

QSize KShortcutsDialog::sizeHint() const
{
    return QSize(600, 480);
}

int KShortcutsDialog::configure(KActionCollection *collection, KShortcutsEditor::LetterShortcuts allowLetterShortcuts, QWidget *parent, bool saveSettings)
{
    KShortcutsDialog dlg(KShortcutsEditor::AllActions, allowLetterShortcuts, parent);
    dlg.d->editor->addCollection(collection);
    return dlg.configure(saveSettings);
}

void KShortcutsDialog::slotDefaults()
{
    d->editor->allDefault();
}
