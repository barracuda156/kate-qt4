/* qt4compat: kkeysequencewidget.cpp */
#include <kkeysequencewidget.h>

#include <kicon.h>
#include <klocale.h>

#include <QtCore/QEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>

class KKeySequenceWidget::Private
{
public:
    Private() : button(0), clearButton(0), recording(false), modifierlessAllowed(false), multiKey(false), types(LocalShortcuts) {}
    QPushButton *button;
    QToolButton *clearButton;
    QKeySequence keySequence;
    QKeySequence oldKeySequence;
    bool recording;
    bool modifierlessAllowed;
    bool multiKey;
    ShortcutTypes types;
    QList<KActionCollection *> collections;
};

KKeySequenceWidget::KKeySequenceWidget(QWidget *parent)
    : QWidget(parent), d(new Private)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    d->button = new QPushButton(this);
    d->button->setFocusPolicy(Qt::StrongFocus);
    d->button->installEventFilter(this);
    layout->addWidget(d->button);
    d->clearButton = new QToolButton(this);
    d->clearButton->setIcon(KIcon(QLatin1String("edit-clear-locationbar-rtl")));
    d->clearButton->setToolTip(i18n("Clear the shortcut"));
    if (d->clearButton->icon().isNull())
        d->clearButton->setText(i18n("Clear"));
    layout->addWidget(d->clearButton);
    connect(d->button, SIGNAL(clicked()), this, SLOT(captureKeySequence()));
    connect(d->clearButton, SIGNAL(clicked()), this, SLOT(clearKeySequence()));
    updateShortcutDisplay();
}

KKeySequenceWidget::~KKeySequenceWidget()
{
    delete d;
}

void KKeySequenceWidget::setModifierlessAllowed(bool allow)
{
    d->modifierlessAllowed = allow;
}

bool KKeySequenceWidget::isModifierlessAllowed()
{
    return d->modifierlessAllowed;
}

void KKeySequenceWidget::setClearButtonShown(bool show)
{
    d->clearButton->setVisible(show);
}

QKeySequence KKeySequenceWidget::keySequence() const
{
    return d->keySequence;
}

void KKeySequenceWidget::setCheckForConflictsAgainst(ShortcutTypes types)
{
    d->types = types;
}

KKeySequenceWidget::ShortcutTypes KKeySequenceWidget::checkForConflictsAgainst() const
{
    return d->types;
}

void KKeySequenceWidget::setMultiKeyShortcutsAllowed(bool allowed)
{
    d->multiKey = allowed;
}

bool KKeySequenceWidget::multiKeyShortcutsAllowed() const
{
    return d->multiKey;
}

void KKeySequenceWidget::setCheckActionCollections(const QList<KActionCollection *> &actionCollections)
{
    d->collections = actionCollections;
}

bool KKeySequenceWidget::isKeySequenceAvailable(const QKeySequence &) const
{
    return true;
}

void KKeySequenceWidget::captureKeySequence()
{
    startRecording();
}

void KKeySequenceWidget::setKeySequence(const QKeySequence &seq, Validation)
{
    d->keySequence = seq;
    d->oldKeySequence = seq;
    updateShortcutDisplay();
    emit keySequenceChanged(d->keySequence);
}

void KKeySequenceWidget::clearKeySequence()
{
    setKeySequence(QKeySequence());
}

void KKeySequenceWidget::applyStealShortcut()
{
}

void KKeySequenceWidget::startRecording()
{
    d->oldKeySequence = d->keySequence;
    d->recording = true;
    d->button->setDown(true);
    d->button->grabKeyboard();
    updateShortcutDisplay();
}

void KKeySequenceWidget::doneRecording()
{
    d->recording = false;
    d->button->releaseKeyboard();
    d->button->setDown(false);
    updateShortcutDisplay();
    if (d->keySequence != d->oldKeySequence)
        emit keySequenceChanged(d->keySequence);
}

void KKeySequenceWidget::updateShortcutDisplay()
{
    if (d->recording) {
        d->button->setText(i18nc("What the user inputs now will be taken as the new shortcut", "Input"));
    } else if (d->keySequence.isEmpty()) {
        d->button->setText(i18nc("No shortcut defined", "None"));
    } else {
        d->button->setText(d->keySequence.toString(QKeySequence::NativeText));
    }
}

bool KKeySequenceWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d->button && d->recording) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            const int key = ke->key();
            if (key == Qt::Key_Shift || key == Qt::Key_Control || key == Qt::Key_Alt || key == Qt::Key_Meta
                || key == Qt::Key_AltGr || key == Qt::Key_unknown || key == -1)
                return true;
            const Qt::KeyboardModifiers mods = ke->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier);
            if (key == Qt::Key_Escape && mods == Qt::NoModifier) {
                /* cancel */
                d->keySequence = d->oldKeySequence;
                doneRecording();
                return true;
            }
            d->keySequence = QKeySequence(int(mods) | key);
            doneRecording();
            return true;
        }
        if (event->type() == QEvent::FocusOut) {
            d->keySequence = d->oldKeySequence;
            doneRecording();
            return false;
        }
    }
    return QWidget::eventFilter(watched, event);
}
