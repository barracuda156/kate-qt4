/* qt4compat: klineedit.h - KLineEdit with KCompletion support, clear button and click message */
#ifndef KDECOMPAT_KLINEEDIT_H
#define KDECOMPAT_KLINEEDIT_H

#include <QtGui/QLineEdit>
#include <kdemacros.h>
#include <kcompletion.h>
#include <kglobalsettings.h>

class QAction;
class QMenu;
class QToolButton;
class KCompletionBox;
class KUrl;

class KDEUI_EXPORT KLineEdit : public QLineEdit, public KCompletionBase
{
    Q_OBJECT
    friend class KComboBox;
    Q_PROPERTY(bool contextMenuEnabled READ isContextMenuEnabled WRITE setContextMenuEnabled)
    Q_PROPERTY(bool urlDropsEnabled READ urlDropsEnabled WRITE setUrlDropsEnabled)
    Q_PROPERTY(bool trapEnterKeyEvent READ trapReturnKey WRITE setTrapReturnKey)
    Q_PROPERTY(bool squeezedTextEnabled READ isSqueezedTextEnabled WRITE setSqueezedTextEnabled)
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
    Q_PROPERTY(bool showClearButton READ isClearButtonShown WRITE setClearButtonShown)
    Q_PROPERTY(bool passwordMode READ passwordMode WRITE setPasswordMode)

public:
    explicit KLineEdit(const QString &string, QWidget *parent = 0);
    explicit KLineEdit(QWidget *parent = 0);
    virtual ~KLineEdit();

    void setUrl(const KUrl &url);
    virtual void setCompletionMode(KGlobalSettings::Completion mode);
    void setCompletionModeDisabled(KGlobalSettings::Completion mode, bool disable = true);
    virtual void setContextMenuEnabled(bool showMenu);
    bool isContextMenuEnabled() const;
    void setUrlDropsEnabled(bool enable);
    bool urlDropsEnabled() const;
    void setTrapReturnKey(bool trap);
    bool trapReturnKey() const;
    KCompletionBox *completionBox(bool create = true);
    virtual void setCompletionObject(KCompletion *, bool hsig = true);
    virtual void copy() const;
    void setSqueezedTextEnabled(bool enable);
    bool isSqueezedTextEnabled() const;
    QString originalText() const;
    QString userText() const;
    void setCompletionBox(KCompletionBox *box);
    void setClickMessage(const QString &msg);
    QString clickMessage() const;
    void setClearButtonShown(bool show);
    bool isClearButtonShown() const;
    QSize clearButtonUsedSize() const;
    void doCompletion(const QString &txt);

Q_SIGNALS:
    void completionBoxActivated(const QString &);
    void returnPressed(const QString &);
    void completion(const QString &);
    void substringCompletion(const QString &);
    void userTextChanged(const QString &);
    void textRotation(KCompletionBase::KeyBindingType);
    void completionModeChanged(KGlobalSettings::Completion);
    void aboutToShowContextMenu(QMenu *menu);
    void clearButtonClicked();

public Q_SLOTS:
    virtual void setReadOnly(bool);
    void rotateText(KCompletionBase::KeyBindingType type);
    virtual void setCompletedText(const QString &);
    void setCompletedItems(const QStringList &items, bool autoSuggest = true);
    virtual void clear();
    void setSqueezedText(const QString &text);
    virtual void setText(const QString &);
    void setPasswordMode(bool b = true);
    bool passwordMode() const;

protected Q_SLOTS:
    virtual void makeCompletion(const QString &);
    void userCancelled(const QString &cancelText);

protected:
    virtual bool event(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *);
    QMenu *createStandardContextMenu();
    virtual void dropEvent(QDropEvent *);
    virtual void setCompletedText(const QString &, bool);
    void setUserSelection(bool userSelection);
    bool autoSuggest() const;
    virtual void paintEvent(QPaintEvent *ev);
    virtual void focusInEvent(QFocusEvent *ev);
    virtual void focusOutEvent(QFocusEvent *ev);

private Q_SLOTS:
    void completionMenuActivated(QAction *act);
    void slotCompletionBoxTextChanged(const QString &text);
    void slotTextChanged(const QString &text);
    void slotClearButtonClicked();
    void slotRestoreSelectionColors();

private:
    void init();
    void updateClearButton();
    void setSqueezedText();
    bool copySqueezedText(bool clipboard) const;

    KCompletionBox *m_completionBox;
    QToolButton *m_clearButton;
    QString m_squeezedText;
    QString m_userText;
    QMap<KGlobalSettings::Completion, bool> m_disableCompletionMap;
    bool m_contextMenuEnabled;
    bool m_urlDropsEnabled;
    bool m_trapReturnKey;
    bool m_squeezedEnabled;
    bool m_clearButtonShown;
    bool m_updatingClearButton;
    bool m_autoSuggest;
    bool m_userSelection;
    bool m_disableRestoreSelection;
    bool m_completionRunning;
    int m_squeezedStart;
    int m_squeezedEnd;
};

#endif
