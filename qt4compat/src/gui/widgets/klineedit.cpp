/* qt4compat: klineedit.cpp - trimmed re-implementation of kdelibs' KLineEdit */
#include <klineedit.h>

#include <kcompletionbox.h>
#include <kicon.h>
#include <klocale.h>
#include <kurl.h>

#include <QtCore/QMimeData>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>

KLineEdit::KLineEdit(const QString &string, QWidget *parent)
    : QLineEdit(string, parent), KCompletionBase()
{
    init();
}

KLineEdit::KLineEdit(QWidget *parent)
    : QLineEdit(parent), KCompletionBase()
{
    init();
}

KLineEdit::~KLineEdit()
{
}

void KLineEdit::init()
{
    m_completionBox = 0;
    m_clearButton = 0;
    m_contextMenuEnabled = true;
    m_urlDropsEnabled = true;
    m_trapReturnKey = false;
    m_squeezedEnabled = false;
    m_clearButtonShown = false;
    m_autoSuggest = false;
    m_userSelection = true;
    m_disableRestoreSelection = false;
    m_completionRunning = false;
    m_squeezedStart = 0;
    m_squeezedEnd = 0;
    setDragEnabled(true);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));
    /* the kdelibs default: completion mode from the desktop settings */
    KCompletionBase::setCompletionMode(KGlobalSettings::completionMode());
}

void KLineEdit::setUrl(const KUrl &url)
{
    setText(url.pathOrUrl());
}

void KLineEdit::setCompletionMode(KGlobalSettings::Completion mode)
{
    KGlobalSettings::Completion oldMode = completionMode();
    if (oldMode != mode && (oldMode == KGlobalSettings::CompletionPopup || oldMode == KGlobalSettings::CompletionPopupAuto)
        && m_completionBox && m_completionBox->isVisible())
        m_completionBox->hide();
    if (m_disableCompletionMap.value(mode, false))
        return;
    KCompletionBase::setCompletionMode(mode);
    emit completionModeChanged(mode);
}

void KLineEdit::setCompletionModeDisabled(KGlobalSettings::Completion mode, bool disable)
{
    m_disableCompletionMap[mode] = disable;
}

void KLineEdit::setContextMenuEnabled(bool showMenu)
{
    m_contextMenuEnabled = showMenu;
}

bool KLineEdit::isContextMenuEnabled() const
{
    return m_contextMenuEnabled;
}

void KLineEdit::setUrlDropsEnabled(bool enable)
{
    m_urlDropsEnabled = enable;
}

bool KLineEdit::urlDropsEnabled() const
{
    return m_urlDropsEnabled;
}

void KLineEdit::setTrapReturnKey(bool trap)
{
    m_trapReturnKey = trap;
}

bool KLineEdit::trapReturnKey() const
{
    return m_trapReturnKey;
}

KCompletionBox *KLineEdit::completionBox(bool create)
{
    if (create && !m_completionBox) {
        setCompletionBox(new KCompletionBox(this));
        m_completionBox->setObjectName(QLatin1String("completion box"));
        m_completionBox->setFont(font());
    }
    return m_completionBox;
}

void KLineEdit::setCompletionObject(KCompletion *comp, bool hsig)
{
    KCompletion *oldComp = compObj();
    if (oldComp && handleSignals())
        disconnect(oldComp, SIGNAL(matches(QStringList)), this, SLOT(setCompletedItems(QStringList)));
    if (comp && hsig)
        connect(comp, SIGNAL(matches(QStringList)), this, SLOT(setCompletedItems(QStringList)));
    KCompletionBase::setCompletionObject(comp, hsig);
}

void KLineEdit::copy() const
{
    if (!copySqueezedText(true))
        QLineEdit::copy();
}

void KLineEdit::setSqueezedTextEnabled(bool enable)
{
    m_squeezedEnabled = enable;
}

bool KLineEdit::isSqueezedTextEnabled() const
{
    return m_squeezedEnabled;
}

QString KLineEdit::originalText() const
{
    if (m_squeezedEnabled)
        return m_squeezedText;
    return text();
}

QString KLineEdit::userText() const
{
    return m_userText;
}

void KLineEdit::setCompletionBox(KCompletionBox *box)
{
    if (m_completionBox)
        return;
    m_completionBox = box;
    if (handleSignals()) {
        connect(m_completionBox, SIGNAL(currentTextChanged(QString)), this, SLOT(slotCompletionBoxTextChanged(QString)));
        connect(m_completionBox, SIGNAL(userCancelled(QString)), this, SLOT(userCancelled(QString)));
        connect(m_completionBox, SIGNAL(activated(QString)), this, SIGNAL(completionBoxActivated(QString)));
        connect(m_completionBox, SIGNAL(activated(QString)), this, SIGNAL(textEdited(QString)));
    }
}

void KLineEdit::setClickMessage(const QString &msg)
{
    setPlaceholderText(msg);
}

QString KLineEdit::clickMessage() const
{
    return placeholderText();
}

void KLineEdit::setClearButtonShown(bool show)
{
    if (show == m_clearButtonShown)
        return;
    m_clearButtonShown = show;
    if (show && !m_clearButton) {
        m_clearButton = new QToolButton(this);
        m_clearButton->setCursor(Qt::ArrowCursor);
        m_clearButton->setFocusPolicy(Qt::NoFocus);
        m_clearButton->setAutoRaise(true);
        m_clearButton->setToolTip(i18n("Clear text"));
        QIcon icon = KIcon(layoutDirection() == Qt::LeftToRight ? QLatin1String("edit-clear-locationbar-rtl") : QLatin1String("edit-clear-locationbar-ltr"));
        if (icon.isNull())
            icon = KIcon(QLatin1String("edit-clear"));
        m_clearButton->setIcon(icon);
        m_clearButton->setIconSize(QSize(16, 16));
        m_clearButton->setStyleSheet(QLatin1String("QToolButton { border: none; padding: 0px; }"));
        connect(m_clearButton, SIGNAL(clicked()), this, SLOT(slotClearButtonClicked()));
    }
    updateClearButton();
}

bool KLineEdit::isClearButtonShown() const
{
    return m_clearButtonShown;
}

QSize KLineEdit::clearButtonUsedSize() const
{
    if (!m_clearButtonShown || !m_clearButton)
        return QSize();
    return m_clearButton->sizeHint();
}

void KLineEdit::updateClearButton()
{
    if (!m_clearButton)
        return;
    const bool visible = m_clearButtonShown && !text().isEmpty() && !isReadOnly();
    m_clearButton->setVisible(visible);
    const int frame = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
    const int padding = visible ? m_clearButton->sizeHint().width() + frame + 2 : 0;
    if (layoutDirection() == Qt::LeftToRight)
        setStyleSheet(QString::fromLatin1("QLineEdit { padding-right: %1px; }").arg(padding));
    else
        setStyleSheet(QString::fromLatin1("QLineEdit { padding-left: %1px; }").arg(padding));
    if (visible) {
        const QSize sz = m_clearButton->sizeHint();
        const QRect r = rect();
        if (layoutDirection() == Qt::LeftToRight)
            m_clearButton->move(r.right() - frame - sz.width() - 1, (r.height() - sz.height()) / 2);
        else
            m_clearButton->move(r.left() + frame + 1, (r.height() - sz.height()) / 2);
    }
}

void KLineEdit::slotClearButtonClicked()
{
    if (!text().isEmpty()) {
        clear();
        emit textEdited(QString());
        emit clearButtonClicked();
    }
}

void KLineEdit::doCompletion(const QString &txt)
{
    if (emitSignals())
        emit completion(txt);
    if (handleSignals())
        makeCompletion(txt);
}

void KLineEdit::setReadOnly(bool readOnly)
{
    if (readOnly == isReadOnly())
        return;
    QLineEdit::setReadOnly(readOnly);
    if (readOnly) {
        QPalette p = palette();
        QColor color = p.color(QPalette::Disabled, QPalette::Background);
        p.setColor(QPalette::Active, QPalette::Base, color);
        p.setColor(QPalette::Inactive, QPalette::Base, color);
        setPalette(p);
    } else {
        setPalette(QPalette());
    }
    updateClearButton();
}

void KLineEdit::rotateText(KCompletionBase::KeyBindingType type)
{
    KCompletion *comp = compObj();
    if (comp && (type == KCompletionBase::PrevCompletionMatch || type == KCompletionBase::NextCompletionMatch)) {
        QString input = (type == KCompletionBase::PrevCompletionMatch) ? comp->previousMatch() : comp->nextMatch();
        if (input.isEmpty() || input == displayText())
            return;
        setCompletedText(input, hasSelectedText());
    }
}

void KLineEdit::setCompletedText(const QString &t, bool marked)
{
    if (!m_autoSuggest)
        return;
    const QString txt = text();
    if (t != txt) {
        setText(t);
        if (marked)
            setSelection(t.length(), txt.length() - t.length());
        setUserSelection(false);
    } else {
        setUserSelection(true);
    }
}

void KLineEdit::setCompletedText(const QString &text)
{
    KGlobalSettings::Completion mode = completionMode();
    const bool marked = (mode == KGlobalSettings::CompletionAuto || mode == KGlobalSettings::CompletionMan
                         || mode == KGlobalSettings::CompletionPopup || mode == KGlobalSettings::CompletionPopupAuto);
    setCompletedText(text, marked);
}

void KLineEdit::setCompletedItems(const QStringList &items, bool autoSuggest)
{
    const QString txt = m_completionBox && m_completionBox->isVisible() ? m_completionBox->cancelledText() : text();
    if (!items.isEmpty() && !(items.count() == 1 && txt == items.first())) {
        if (m_completionBox && !m_completionBox->isVisible()) {
            m_completionBox->setCancelledText(txt);
        } else if (!m_completionBox) {
            completionBox()->setCancelledText(txt);
        }
        m_completionBox->setItems(items);
        m_completionBox->setTabHandling(true);
        m_completionBox->popup();
        if (autoSuggest) {
            const int index = items.first().indexOf(txt);
            const QString newText = items.first().mid(index);
            setUserSelection(false);
            m_autoSuggest = true;
            setCompletedText(newText, true);
            m_autoSuggest = false;
        }
    } else if (m_completionBox && m_completionBox->isVisible()) {
        m_completionBox->hide();
    }
}

void KLineEdit::clear()
{
    setText(QString());
}

void KLineEdit::setSqueezedText(const QString &text)
{
    setSqueezedTextEnabled(true);
    setText(text);
}

void KLineEdit::setSqueezedText()
{
    const QString fullText = m_squeezedText;
    const int fullLength = fullText.length();
    const QFontMetrics fm(fontMetrics());
    const int labelWidth = size().width() - 2 * style()->pixelMetric(QStyle::PM_DefaultFrameWidth) - 2;
    const int textWidth = fm.width(fullText);
    if (textWidth > labelWidth) {
        int letters = fullText.length() * (labelWidth - fm.width(QLatin1String("..."))) / textWidth / 2;
        QString squeezedText = fullText.left(letters) + QLatin1String("...") + fullText.right(letters);
        int squeezedWidth = fm.width(squeezedText);
        if (squeezedWidth > labelWidth) {
            do {
                letters--;
                squeezedText = fullText.left(letters) + QLatin1String("...") + fullText.right(letters);
                squeezedWidth = fm.width(squeezedText);
            } while (squeezedWidth > labelWidth && letters > 1);
        } else if (squeezedWidth < labelWidth) {
            do {
                letters++;
                squeezedText = fullText.left(letters) + QLatin1String("...") + fullText.right(letters);
                squeezedWidth = fm.width(squeezedText);
            } while (squeezedWidth < labelWidth && letters < fullLength / 2);
            letters--;
            squeezedText = fullText.left(letters) + QLatin1String("...") + fullText.right(letters);
        }
        m_squeezedStart = letters;
        m_squeezedEnd = fullLength - letters;
        QLineEdit::setText(squeezedText);
        setToolTip(fullText);
        setCursorPosition(0);
    } else {
        m_squeezedStart = 0;
        m_squeezedEnd = 0;
        QLineEdit::setText(fullText);
        setToolTip(QString());
    }
}

bool KLineEdit::copySqueezedText(bool clipboard) const
{
    if (!m_squeezedEnabled || m_squeezedText.isEmpty())
        return false;
    QString t = m_squeezedText;
    if (hasSelectedText()) {
        int start = selectionStart();
        int end = start + selectedText().length();
        if (start >= m_squeezedStart + 3)
            start = start - 3 - m_squeezedStart + m_squeezedEnd;
        else if (start > m_squeezedStart)
            start = m_squeezedStart;
        if (end >= m_squeezedStart + 3)
            end = end - 3 - m_squeezedStart + m_squeezedEnd;
        else if (end > m_squeezedStart)
            end = m_squeezedEnd;
        t = t.mid(start, end - start);
    }
    QApplication::clipboard()->setText(t, clipboard ? QClipboard::Clipboard : QClipboard::Selection);
    return true;
}

void KLineEdit::setText(const QString &text)
{
    if (m_squeezedEnabled) {
        m_squeezedText = text;
        setSqueezedText();
        return;
    }
    QLineEdit::setText(text);
}

void KLineEdit::setPasswordMode(bool b)
{
    setEchoMode(b ? QLineEdit::Password : QLineEdit::Normal);
}

bool KLineEdit::passwordMode() const
{
    return echoMode() == QLineEdit::Password;
}

void KLineEdit::makeCompletion(const QString &text)
{
    KCompletion *comp = compObj();
    KGlobalSettings::Completion mode = completionMode();
    if (!comp || mode == KGlobalSettings::CompletionNone)
        return;
    const QString match = comp->makeCompletion(text);
    if (mode == KGlobalSettings::CompletionPopup || mode == KGlobalSettings::CompletionPopupAuto) {
        if (match.isEmpty()) {
            if (m_completionBox) {
                m_completionBox->hide();
                m_completionBox->clear();
            }
        } else {
            setCompletedItems(comp->allMatches(), mode == KGlobalSettings::CompletionPopupAuto);
        }
    } else {
        if (match.isEmpty() || match == text)
            return;
        if (mode != KGlobalSettings::CompletionShell)
            setUserSelection(false);
        m_autoSuggest = true;
        setCompletedText(match);
        m_autoSuggest = false;
    }
}

void KLineEdit::userCancelled(const QString &cancelText)
{
    if (completionMode() != KGlobalSettings::CompletionPopupAuto) {
        setText(cancelText);
    } else if (hasSelectedText()) {
        if (m_userSelection) {
            deselect();
        } else {
            m_autoSuggest = false;
            const int start = selectionStart();
            const QString s = text().remove(selectionStart(), selectedText().length());
            setText(s);
            setCursorPosition(start);
            m_autoSuggest = true;
        }
    }
}

bool KLineEdit::event(QEvent *ev)
{
    if (ev->type() == QEvent::ShortcutOverride) {
        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        if (m_trapReturnKey && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)) {
            e->accept();
            return true;
        }
    } else if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(ev);
        if (e->key() == Qt::Key_Tab && completionMode() == KGlobalSettings::CompletionShell) {
            keyPressEvent(e);
            if (e->isAccepted())
                return true;
        }
    } else if (ev->type() == QEvent::LayoutDirectionChange || ev->type() == QEvent::StyleChange) {
        updateClearButton();
    }
    return QLineEdit::event(ev);
}

void KLineEdit::resizeEvent(QResizeEvent *ev)
{
    if (m_squeezedEnabled)
        setSqueezedText();
    updateClearButton();
    QLineEdit::resizeEvent(ev);
}

void KLineEdit::keyPressEvent(QKeyEvent *e)
{
    const int key = e->key() | e->modifiers();

    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        QLineEdit::keyPressEvent(e);
        if (m_completionBox && m_completionBox->isVisible())
            m_completionBox->hide();
        emit returnPressed(displayText());
        if (m_trapReturnKey)
            e->accept();
        return;
    }

    KGlobalSettings::Completion mode = completionMode();
    const bool noModifier = (e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ShiftModifier || e->modifiers() == Qt::KeypadModifier);

    if ((mode == KGlobalSettings::CompletionAuto || mode == KGlobalSettings::CompletionPopupAuto || mode == KGlobalSettings::CompletionMan) && noModifier) {
        if (!m_userSelection && hasSelectedText() && (e->key() == Qt::Key_Right || e->key() == Qt::Key_End) && !isReadOnly()) {
            QLineEdit::keyPressEvent(e);
            setUserSelection(true);
            return;
        }
        if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) {
            QLineEdit::keyPressEvent(e);
            if (m_completionBox && m_completionBox->isVisible() && text().isEmpty())
                m_completionBox->hide();
            return;
        }
    }

    if (mode == KGlobalSettings::CompletionShell && e->key() == Qt::Key_Tab && !isReadOnly()) {
        const QString txt = text();
        const int len = txt.length();
        if (cursorPosition() == len && len != 0) {
            doCompletion(txt);
            e->accept();
            return;
        }
    }

    if ((mode == KGlobalSettings::CompletionPopup || mode == KGlobalSettings::CompletionPopupAuto) && !isReadOnly()) {
        if (e->key() == Qt::Key_Down || e->key() == Qt::Key_Up) {
            if (m_completionBox && m_completionBox->isVisible()) {
                QApplication::sendEvent(m_completionBox, e);
                return;
            }
            if (e->key() == Qt::Key_Down && (e->modifiers() & Qt::ControlModifier)) {
                doCompletion(text());
                e->accept();
                return;
            }
        }
        if (e->key() == Qt::Key_Escape && m_completionBox && m_completionBox->isVisible()) {
            m_completionBox->hide();
            e->accept();
            return;
        }
    }

    /* Ctrl+E / Ctrl+T style rotations, as in kdelibs' default key bindings */
    if (e->modifiers() == Qt::ControlModifier && (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)) {
        if (compObj() && mode != KGlobalSettings::CompletionNone) {
            rotateText(e->key() == Qt::Key_Up ? KCompletionBase::PrevCompletionMatch : KCompletionBase::NextCompletionMatch);
            if (emitSignals())
                emit textRotation(e->key() == Qt::Key_Up ? KCompletionBase::PrevCompletionMatch : KCompletionBase::NextCompletionMatch);
            e->accept();
            return;
        }
    }

    QLineEdit::keyPressEvent(e);

    if (!e->text().isEmpty() && !e->text().at(0).isPrint() && e->key() != Qt::Key_Backspace)
        return;

    const bool editingKey = (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete || (!e->text().isEmpty() && e->text().at(0).isPrint()));
    if (editingKey && !isReadOnly() && mode != KGlobalSettings::CompletionNone && mode != KGlobalSettings::CompletionShell) {
        const QString txt = text();
        if (!txt.isEmpty() || (m_completionBox && m_completionBox->isVisible())) {
            if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) {
                if (mode == KGlobalSettings::CompletionPopup || mode == KGlobalSettings::CompletionPopupAuto) {
                    if (txt.isEmpty() && m_completionBox)
                        m_completionBox->hide();
                    else
                        doCompletion(txt);
                }
                /* no auto completion on deletion for Auto/Man modes */
            } else {
                m_autoSuggest = (mode != KGlobalSettings::CompletionMan);
                doCompletion(txt);
                m_autoSuggest = false;
            }
        } else if (m_completionBox) {
            m_completionBox->hide();
        }
    }
}

void KLineEdit::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && !m_userSelection && hasSelectedText())
        setUserSelection(true);
    QLineEdit::mousePressEvent(e);
}

void KLineEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QLineEdit::mouseReleaseEvent(e);
    if (QApplication::clipboard()->supportsSelection() && e->button() == Qt::LeftButton)
        copySqueezedText(false);
}

void KLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QLineEdit::mouseDoubleClickEvent(e);
}

QMenu *KLineEdit::createStandardContextMenu()
{
    QMenu *popup = QLineEdit::createStandardContextMenu();
    if (!isReadOnly()) {
        if (m_clearButtonShown) {
            QAction *clearAllAction = new QAction(KIcon(QLatin1String("edit-clear")), i18n("C&lear"), popup);
            connect(clearAllAction, SIGNAL(triggered()), this, SLOT(clear()));
            clearAllAction->setEnabled(!text().isEmpty());
            popup->addSeparator();
            popup->addAction(clearAllAction);
        }
        if (compObj()) {
            QMenu *subMenu = popup->addMenu(KIcon(QLatin1String("text-completion")), i18nc("@title:menu", "Text Completion"));
            connect(subMenu, SIGNAL(triggered(QAction*)), this, SLOT(completionMenuActivated(QAction*)));
            popup->addSeparator();
            QActionGroup *ag = new QActionGroup(subMenu);
            struct Entry { const char *text; KGlobalSettings::Completion mode; };
            const Entry entries[] = {
                { "None", KGlobalSettings::CompletionNone },
                { "Manual", KGlobalSettings::CompletionShell },
                { "Automatic", KGlobalSettings::CompletionAuto },
                { "Dropdown List", KGlobalSettings::CompletionPopup },
                { "Short Automatic", KGlobalSettings::CompletionMan },
                { "Dropdown List && Automatic", KGlobalSettings::CompletionPopupAuto }
            };
            const KGlobalSettings::Completion mode = completionMode();
            for (unsigned i = 0; i < sizeof(entries) / sizeof(entries[0]); ++i) {
                QAction *a = subMenu->addAction(i18nc("@item:inmenu Text Completion", entries[i].text));
                a->setCheckable(true);
                a->setData(int(entries[i].mode));
                a->setChecked(mode == entries[i].mode);
                ag->addAction(a);
            }
        }
    }
    return popup;
}

void KLineEdit::contextMenuEvent(QContextMenuEvent *e)
{
    if (!m_contextMenuEnabled)
        return;
    QMenu *menu = createStandardContextMenu();
    emit aboutToShowContextMenu(menu);
    menu->exec(e->globalPos());
    delete menu;
}

void KLineEdit::completionMenuActivated(QAction *act)
{
    if (!act || !act->data().isValid())
        return;
    setCompletionMode(static_cast<KGlobalSettings::Completion>(act->data().toInt()));
}

void KLineEdit::dropEvent(QDropEvent *e)
{
    if (m_urlDropsEnabled && KUrl::List::canDecode(e->mimeData())) {
        const KUrl::List urls = KUrl::List::fromMimeData(e->mimeData());
        if (!urls.isEmpty()) {
            QString dropText = text();
            for (KUrl::List::ConstIterator it = urls.constBegin(); it != urls.constEnd(); ++it) {
                if (!dropText.isEmpty())
                    dropText += QLatin1Char(' ');
                dropText += (*it).pathOrUrl();
            }
            setText(dropText);
            setCursorPosition(dropText.length());
            e->accept();
            return;
        }
    }
    QLineEdit::dropEvent(e);
}

void KLineEdit::setUserSelection(bool userSelection)
{
    m_userSelection = userSelection;
    QPalette p = palette();
    if (userSelection) {
        p.setColor(QPalette::Highlight, QPalette().color(QPalette::Highlight));
        p.setColor(QPalette::HighlightedText, QPalette().color(QPalette::HighlightedText));
    } else {
        QColor color = p.color(QPalette::Disabled, QPalette::Text);
        p.setColor(QPalette::HighlightedText, color);
        color = p.color(QPalette::Active, QPalette::Base);
        p.setColor(QPalette::Highlight, color);
    }
    setPalette(p);
}

bool KLineEdit::autoSuggest() const
{
    return m_autoSuggest;
}

void KLineEdit::paintEvent(QPaintEvent *ev)
{
    QLineEdit::paintEvent(ev);
}

void KLineEdit::focusInEvent(QFocusEvent *ev)
{
    QLineEdit::focusInEvent(ev);
}

void KLineEdit::focusOutEvent(QFocusEvent *ev)
{
    QLineEdit::focusOutEvent(ev);
}

void KLineEdit::slotCompletionBoxTextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        setText(text);
        setModified(true);
        end(false);
    }
}

void KLineEdit::slotTextChanged(const QString &text)
{
    updateClearButton();
    if (!m_completionRunning)
        m_userText = text;
    if (!m_completionRunning)
        emit userTextChanged(text);
}

void KLineEdit::slotRestoreSelectionColors()
{
    setUserSelection(m_userSelection);
}
