/* qt4compat: ktextedit.cpp */
#include <ktextedit.h>

#include <QtGui/QContextMenuEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QPainter>

KTextEdit::KTextEdit(const QString &text, QWidget *parent)
    : QTextEdit(text, parent), m_spell(false), m_tabChangesFocusOnReadOnly(false)
{
}

KTextEdit::KTextEdit(QWidget *parent)
    : QTextEdit(parent), m_spell(false), m_tabChangesFocusOnReadOnly(false)
{
}

KTextEdit::~KTextEdit()
{
}

void KTextEdit::setReadOnly(bool readOnly)
{
    QTextEdit::setReadOnly(readOnly);
    if (readOnly) {
        m_tabChangesFocusOnReadOnly = tabChangesFocus();
        setTabChangesFocus(true);
    } else {
        setTabChangesFocus(m_tabChangesFocusOnReadOnly);
    }
}

void KTextEdit::setCheckSpellingEnabled(bool check)
{
    if (m_spell == check)
        return;
    m_spell = check;
    emit checkSpellingChanged(check);
}

bool KTextEdit::checkSpellingEnabled() const
{
    return m_spell;
}

void KTextEdit::setSpellCheckingLanguage(const QString &language)
{
    if (m_language == language)
        return;
    m_language = language;
    emit languageChanged(language);
}

const QString &KTextEdit::spellCheckingLanguage() const
{
    return m_language;
}

void KTextEdit::setClickMessage(const QString &msg)
{
    m_clickMessage = msg;
    viewport()->update();
}

QString KTextEdit::clickMessage() const
{
    return m_clickMessage;
}

void KTextEdit::setFindReplaceEnabled(bool)
{
}

void KTextEdit::setShowTabAction(bool)
{
}

void KTextEdit::enableFindReplace(bool)
{
}

void KTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    if (!m_clickMessage.isEmpty() && document()->isEmpty() && !hasFocus()) {
        QPainter p(viewport());
        QColor color = palette().text().color();
        color.setAlphaF(0.5);
        p.setPen(color);
        p.drawText(viewport()->rect().adjusted(4, 4, -4, -4), Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, m_clickMessage);
    }
}

void KTextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
}

void KTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu(event->pos());
    emit aboutToShowContextMenu(menu);
    menu->exec(event->globalPos());
    delete menu;
}
