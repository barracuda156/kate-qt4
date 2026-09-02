/* qt4compat: sonnet/dialog.h - the interactive spell check dialog. Without a
 * backend it reports that spell checking is unavailable and finishes. */
#ifndef KDECOMPAT_SONNET_DIALOG_H
#define KDECOMPAT_SONNET_DIALOG_H

#include <QtGui/QDialog>
#include <kdemacros.h>

namespace Sonnet
{
    class BackgroundChecker;

    class KDEUI_EXPORT Dialog : public QDialog
    {
        Q_OBJECT
    public:
        Dialog(BackgroundChecker *checker, QWidget *parent);
        ~Dialog();

        QString originalBuffer() const;
        QString buffer() const;
        void show();
        void activeAutoCorrect(bool active);
        void showProgressDialog(int timeout = 500);
        void showSpellCheckCompletionMessage(bool b = true);
        void setSpellCheckContinuedAfterReplacement(bool b);

    public Q_SLOTS:
        void setBuffer(const QString &buffer);

    Q_SIGNALS:
        void done(const QString &newBuffer);
        void misspelling(const QString &word, int start);
        void replace(const QString &oldWord, int start, const QString &newWord);
        void stop();
        void cancel();
        void autoCorrect(const QString &currentWord, const QString &replaceWord);
        void spellCheckStatus(const QString &status);
        void languageChanged(const QString &language);

    private Q_SLOTS:
        void slotFinish();

    private:
        class Private;
        Private *const d;
        Q_DISABLE_COPY(Dialog)
    };
}

#endif
