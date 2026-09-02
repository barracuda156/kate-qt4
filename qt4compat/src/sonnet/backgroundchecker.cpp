/* qt4compat: sonnet/backgroundchecker.cpp */
#include <sonnet/backgroundchecker.h>

#include <QtCore/QTimer>

namespace Sonnet {

class BackgroundChecker::Private
{
public:
    Private() : running(false) {}
    Speller speller;
    QString text;
    bool running;
};

BackgroundChecker::BackgroundChecker(QObject *parent)
    : QObject(parent), d(new Private)
{
}

BackgroundChecker::BackgroundChecker(const Speller &speller, QObject *parent)
    : QObject(parent), d(new Private)
{
    d->speller = speller;
}

BackgroundChecker::~BackgroundChecker()
{
    delete d;
}

void BackgroundChecker::setText(const QString &text)
{
    d->text = text;
    start();
}

QString BackgroundChecker::text() const { return d->text; }
QString BackgroundChecker::currentContext() const { return d->text; }
Speller BackgroundChecker::speller() const { return d->speller; }
void BackgroundChecker::setSpeller(const Speller &speller) { d->speller = speller; }
bool BackgroundChecker::checkWord(const QString &word) { return d->speller.isCorrect(word); }
QStringList BackgroundChecker::suggest(const QString &word) const { return d->speller.suggest(word); }
bool BackgroundChecker::addWordToPersonal(const QString &word) { return d->speller.addToPersonal(word); }
void BackgroundChecker::restore(KConfig *config) { d->speller.restore(config); }

void BackgroundChecker::start()
{
    if (d->running)
        return;
    d->running = true;
    QTimer::singleShot(0, this, SLOT(slotEngineDone()));
}

void BackgroundChecker::stop()
{
    d->running = false;
}

void BackgroundChecker::replace(int start, const QString &oldText, const QString &newText)
{
    if (start >= 0 && start + oldText.length() <= d->text.length())
        d->text.replace(start, oldText.length(), newText);
}

void BackgroundChecker::changeLanguage(const QString &lang)
{
    d->speller.setLanguage(lang);
}

void BackgroundChecker::continueChecking()
{
    if (!d->running) {
        d->running = true;
        QTimer::singleShot(0, this, SLOT(slotEngineDone()));
    }
}

QString BackgroundChecker::fetchMoreText()
{
    return QString();
}

void BackgroundChecker::finishedCurrentFeed()
{
}

void BackgroundChecker::slotEngineDone()
{
    if (!d->running)
        return;
    d->running = false;
    finishedCurrentFeed();
    emit done();
}

} // namespace Sonnet
