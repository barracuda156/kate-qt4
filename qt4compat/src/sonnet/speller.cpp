/* qt4compat: sonnet/speller.cpp */
#include <sonnet/speller.h>

namespace Sonnet {

class Speller::Private
{
public:
    Private() : checkUppercase(true), skipRunTogether(true) {}
    QString language;
    QString defaultLanguage;
    QString client;
    bool checkUppercase;
    bool skipRunTogether;
};

Speller::Speller(const QString &lang)
    : d(new Private)
{
    d->language = lang;
}

Speller::~Speller()
{
    delete d;
}

Speller::Speller(const Speller &speller)
    : d(new Private(*speller.d))
{
}

Speller &Speller::operator=(const Speller &speller)
{
    if (this != &speller)
        *d = *speller.d;
    return *this;
}

bool Speller::isValid() const { return false; }
void Speller::setLanguage(const QString &lang) { d->language = lang; }
QString Speller::language() const { return d->language; }
bool Speller::isCorrect(const QString &) const { return true; }
bool Speller::isMisspelled(const QString &) const { return false; }
QStringList Speller::suggest(const QString &) const { return QStringList(); }

bool Speller::checkAndSuggest(const QString &, QStringList &suggestions) const
{
    suggestions.clear();
    return true;
}

bool Speller::storeReplacement(const QString &, const QString &) { return false; }
bool Speller::addToPersonal(const QString &) { return false; }
bool Speller::addToSession(const QString &) { return false; }
void Speller::save(KConfig *) {}
void Speller::restore(KConfig *) {}
QStringList Speller::availableBackends() const { return QStringList(); }
QStringList Speller::availableLanguages() const { return QStringList(); }
QStringList Speller::availableLanguageNames() const { return QStringList(); }
QMap<QString, QString> Speller::availableDictionaries() const { return QMap<QString, QString>(); }
void Speller::setDefaultLanguage(const QString &lang) { d->defaultLanguage = lang; }

QString Speller::defaultLanguage() const
{
    return d->defaultLanguage.isEmpty() ? QLatin1String("en_US") : d->defaultLanguage;
}

void Speller::setDefaultClient(const QString &client) { d->client = client; }
QString Speller::defaultClient() const { return d->client; }

void Speller::setAttribute(Attribute attr, bool b)
{
    if (attr == CheckUppercase)
        d->checkUppercase = b;
    else
        d->skipRunTogether = b;
}

bool Speller::testAttribute(Attribute attr) const
{
    return attr == CheckUppercase ? d->checkUppercase : d->skipRunTogether;
}

} // namespace Sonnet
