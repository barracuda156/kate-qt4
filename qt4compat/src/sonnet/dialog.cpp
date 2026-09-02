/* qt4compat: sonnet/dialog.cpp */
#include <sonnet/dialog.h>
#include <sonnet/backgroundchecker.h>

#include <klocalizedstring.h>

#include <QtCore/QTimer>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

namespace Sonnet {

class Dialog::Private
{
public:
    Private() : checker(0), messageShown(false), showCompletion(false), continueAfterReplacement(true) {}
    BackgroundChecker *checker;
    QString original;
    QString buffer;
    bool messageShown;
    bool showCompletion;
    bool continueAfterReplacement;
};

Dialog::Dialog(BackgroundChecker *checker, QWidget *parent)
    : QDialog(parent), d(new Private)
{
    d->checker = checker;
    setWindowTitle(i18nc("@title:window", "Check Spelling"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel(i18n("Spell checking is not available in this build of the editor."), this);
    label->setWordWrap(true);
    layout->addWidget(label);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttons, SIGNAL(rejected()), this, SLOT(slotFinish()));
    layout->addWidget(buttons);
}

Dialog::~Dialog()
{
    delete d;
}

QString Dialog::originalBuffer() const { return d->original; }
QString Dialog::buffer() const { return d->buffer; }

void Dialog::show()
{
    if (!d->messageShown) {
        d->messageShown = true;
        QDialog::show();
        return;
    }
    /* subsequent ranges: finish silently */
    QTimer::singleShot(0, this, SLOT(slotFinish()));
}

void Dialog::activeAutoCorrect(bool) {}
void Dialog::showProgressDialog(int) {}
void Dialog::showSpellCheckCompletionMessage(bool b) { d->showCompletion = b; }
void Dialog::setSpellCheckContinuedAfterReplacement(bool b) { d->continueAfterReplacement = b; }

void Dialog::setBuffer(const QString &buffer)
{
    d->original = buffer;
    d->buffer = buffer;
}

void Dialog::slotFinish()
{
    if (isVisible())
        QDialog::hide();
    emit done(d->buffer);
}

} // namespace Sonnet
