/* qt4compat: sonnet/speller.h - Sonnet API without a spell checking backend.
 * Every word is correct and there are no dictionaries. A native backend
 * (NSSpellChecker on macOS) can be plugged in behind this class later. */
#ifndef KDECOMPAT_SONNET_SPELLER_H
#define KDECOMPAT_SONNET_SPELLER_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>

class KConfig;

namespace Sonnet
{
    class KDECORE_EXPORT Speller
    {
    public:
        Speller(const QString &lang = QString());
        ~Speller();
        Speller(const Speller &speller);
        Speller &operator=(const Speller &speller);

        bool isValid() const;
        void setLanguage(const QString &lang);
        QString language() const;
        bool isCorrect(const QString &word) const;
        bool isMisspelled(const QString &word) const;
        QStringList suggest(const QString &word) const;
        bool checkAndSuggest(const QString &word, QStringList &suggestions) const;
        bool storeReplacement(const QString &bad, const QString &good);
        bool addToPersonal(const QString &word);
        bool addToSession(const QString &word);

        enum Attribute { CheckUppercase, SkipRunTogether };
        void save(KConfig *config);
        void restore(KConfig *config);
        QStringList availableBackends() const;
        QStringList availableLanguages() const;
        QStringList availableLanguageNames() const;
        QMap<QString, QString> availableDictionaries() const;
        void setDefaultLanguage(const QString &lang);
        QString defaultLanguage() const;
        void setDefaultClient(const QString &client);
        QString defaultClient() const;
        void setAttribute(Attribute attr, bool b = true);
        bool testAttribute(Attribute attr) const;

    private:
        class Private;
        Private *const d;
    };
}

#endif
