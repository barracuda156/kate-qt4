/* qt4compat: kshortcutseditor.h - simple tree based shortcut editor */
#ifndef KDECOMPAT_KSHORTCUTSEDITOR_H
#define KDECOMPAT_KSHORTCUTSEDITOR_H

#include <QtGui/QWidget>
#include <kdemacros.h>

class KActionCollection;
class KConfig;
class KConfigBase;
class KConfigGroup;
class KShortcutsEditorPrivate;
class QTreeWidgetItem;

class KDEUI_EXPORT KShortcutsEditor : public QWidget
{
    Q_OBJECT
public:
    enum ActionType {
        WidgetAction = Qt::WidgetShortcut,
        WindowAction = Qt::WindowShortcut,
        ApplicationAction = Qt::ApplicationShortcut,
        GlobalAction = 4,
        AllActions = 0xffffffff
    };
    Q_DECLARE_FLAGS(ActionTypes, ActionType)

    enum LetterShortcuts {
        LetterShortcutsDisallowed = 0,
        LetterShortcutsAllowed
    };

    KShortcutsEditor(KActionCollection *collection, QWidget *parent, ActionTypes actionTypes = AllActions,
                     LetterShortcuts allowLetterShortcuts = LetterShortcutsAllowed);
    explicit KShortcutsEditor(QWidget *parent, ActionTypes actionTypes = AllActions,
                              LetterShortcuts allowLetterShortcuts = LetterShortcutsAllowed);
    virtual ~KShortcutsEditor();

    bool isModified() const;
    void clearCollections();
    void addCollection(KActionCollection *, const QString &title = QString());
    void undoChanges();
    void save();
    void commit();
    void exportConfiguration(KConfig *config) const;
    void importConfiguration(KConfig *config);

Q_SIGNALS:
    void keyChange();

public Q_SLOTS:
    void allDefault();
    void printShortcuts() const;

private Q_SLOTS:
    void itemActivated(QTreeWidgetItem *item, int column);

private:
    KShortcutsEditorPrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KShortcutsEditor::ActionTypes)

#endif
