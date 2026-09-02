/* qt4compat: kkeysequencewidget.h - key capture button (single chord) */
#ifndef KDECOMPAT_KKEYSEQUENCEWIDGET_H
#define KDECOMPAT_KKEYSEQUENCEWIDGET_H

#include <QtCore/QList>
#include <QtGui/QKeySequence>
#include <QtGui/QWidget>
#include <kdemacros.h>

class KActionCollection;

class KDEUI_EXPORT KKeySequenceWidget : public QWidget
{
    Q_OBJECT
public:
    enum Validation { Validate = 0, NoValidate = 1 };
    enum ShortcutType { LocalShortcuts = 0x1, StandardShortcuts = 0x2, GlobalShortcuts = 0x4 };
    Q_DECLARE_FLAGS(ShortcutTypes, ShortcutType)

    explicit KKeySequenceWidget(QWidget *parent = 0);
    virtual ~KKeySequenceWidget();

    void setModifierlessAllowed(bool allow);
    bool isModifierlessAllowed();
    void setClearButtonShown(bool show);
    QKeySequence keySequence() const;
    void setCheckForConflictsAgainst(ShortcutTypes types);
    ShortcutTypes checkForConflictsAgainst() const;
    void setMultiKeyShortcutsAllowed(bool);
    bool multiKeyShortcutsAllowed() const;
    void setCheckActionCollections(const QList<KActionCollection *> &actionCollections);
    bool isKeySequenceAvailable(const QKeySequence &seq) const;

Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &seq);

public Q_SLOTS:
    void captureKeySequence();
    void setKeySequence(const QKeySequence &seq, Validation validate = Validate);
    void clearKeySequence();
    void applyStealShortcut();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private Q_SLOTS:
    void startRecording();
    void doneRecording();

private:
    void updateShortcutDisplay();
    class Private;
    Private *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KKeySequenceWidget::ShortcutTypes)

#endif
