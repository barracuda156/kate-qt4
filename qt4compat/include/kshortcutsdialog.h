/* qt4compat: kshortcutsdialog.h */
#ifndef KDECOMPAT_KSHORTCUTSDIALOG_H
#define KDECOMPAT_KSHORTCUTSDIALOG_H

#include <QtGui/QDialog>
#include <kdemacros.h>
#include <kshortcutseditor.h>

class KDEUI_EXPORT KShortcutsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KShortcutsDialog(KShortcutsEditor::ActionTypes types = KShortcutsEditor::AllActions,
                              KShortcutsEditor::LetterShortcuts allowLetterShortcuts = KShortcutsEditor::LetterShortcutsAllowed,
                              QWidget *parent = 0);
    virtual ~KShortcutsDialog();

    void addCollection(KActionCollection *, const QString &title = QString());
    QList<KActionCollection *> actionCollections() const;
    bool configure(bool saveSettings = true);
    virtual QSize sizeHint() const;
    static int configure(KActionCollection *collection,
                         KShortcutsEditor::LetterShortcuts allowLetterShortcuts = KShortcutsEditor::LetterShortcutsAllowed,
                         QWidget *parent = 0, bool bSaveSettings = true);

private Q_SLOTS:
    void slotDefaults();

private:
    class Private;
    Private *const d;
};

#endif
