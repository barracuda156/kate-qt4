/* qt4compat: ktextedit.h - KTextEdit without spell checking and find/replace */
#ifndef KDECOMPAT_KTEXTEDIT_H
#define KDECOMPAT_KTEXTEDIT_H

#include <QtGui/QTextEdit>
#include <kdemacros.h>

class KDEUI_EXPORT KTextEdit : public QTextEdit
{
    Q_OBJECT
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
    Q_PROPERTY(bool checkSpellingEnabled READ checkSpellingEnabled WRITE setCheckSpellingEnabled)
public:
    explicit KTextEdit(const QString &text, QWidget *parent = 0);
    explicit KTextEdit(QWidget *parent = 0);
    ~KTextEdit();

    virtual void setReadOnly(bool readOnly);
    void setCheckSpellingEnabled(bool check);
    bool checkSpellingEnabled() const;
    void setSpellCheckingLanguage(const QString &language);
    const QString &spellCheckingLanguage() const;
    void setClickMessage(const QString &msg);
    QString clickMessage() const;
    void setSpellInterface(void *) {}
    void setCheckSpellingEnabledInternal(bool) {}
    void setFindReplaceEnabled(bool enabled);
    void setShowTabAction(bool show);
    void setShowAutoCorrectionButton(bool) {}
    void enableFindReplace(bool enabled);

Q_SIGNALS:
    void checkSpellingChanged(bool);
    void languageChanged(const QString &language);
    void aboutToShowContextMenu(QMenu *menu);

public Q_SLOTS:
    void checkSpelling() {}
    void showSpellConfigDialog(const QString &) {}
    void showTabAction(bool) {}

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QString m_clickMessage;
    QString m_language;
    bool m_spell;
    bool m_tabChangesFocusOnReadOnly;
};

#endif
