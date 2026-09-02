/* qt4compat: sonnet/backgroundchecker.h - finishes immediately, finds nothing */
#ifndef KDECOMPAT_SONNET_BACKGROUNDCHECKER_H
#define KDECOMPAT_SONNET_BACKGROUNDCHECKER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <sonnet/speller.h>

class KConfig;

namespace Sonnet
{
    class KDECORE_EXPORT BackgroundChecker : public QObject
    {
        Q_OBJECT
    public:
        explicit BackgroundChecker(QObject *parent = 0);
        explicit BackgroundChecker(const Speller &speller, QObject *parent = 0);
        ~BackgroundChecker();

        void setText(const QString &text);
        QString text() const;
        QString currentContext() const;
        Speller speller() const;
        void setSpeller(const Speller &speller);
        bool checkWord(const QString &word);
        QStringList suggest(const QString &word) const;
        bool addWordToPersonal(const QString &word);
        void restore(KConfig *config);

    public Q_SLOTS:
        virtual void start();
        virtual void stop();
        void replace(int start, const QString &oldText, const QString &newText);
        void changeLanguage(const QString &lang);
        virtual void continueChecking();

    Q_SIGNALS:
        void misspelling(const QString &word, int start);
        void done();

    protected:
        virtual QString fetchMoreText();
        virtual void finishedCurrentFeed();

    protected Q_SLOTS:
        void slotEngineDone();

    private:
        class Private;
        Private *const d;
    };
}

#endif
