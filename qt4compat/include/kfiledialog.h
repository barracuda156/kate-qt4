/* qt4compat: kfiledialog.h - static KFileDialog API over QFileDialog (local files only) */
#ifndef KDECOMPAT_KFILEDIALOG_H
#define KDECOMPAT_KFILEDIALOG_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFileDialog>
#include <kdemacros.h>
#include <kurl.h>

class KDEUI_EXPORT KFileDialog
{
public:
    enum OperationMode { Other = 0, Opening, Saving };
    enum Option { ConfirmOverwrite = 0x01, ShowInlinePreview = 0x02 };
    Q_DECLARE_FLAGS(Options, Option)

    static QString getOpenFileName(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                                   QWidget *parent = 0, const QString &caption = QString());
    static QStringList getOpenFileNames(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                                        QWidget *parent = 0, const QString &caption = QString());
    static KUrl getOpenUrl(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                           QWidget *parent = 0, const QString &caption = QString());
    static KUrl::List getOpenUrls(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                                  QWidget *parent = 0, const QString &caption = QString());
    static QString getSaveFileName(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                                   QWidget *parent = 0, const QString &caption = QString());
    static QString getSaveFileName(const KUrl &startDir, const QString &filter, QWidget *parent,
                                   const QString &caption, Options options);
    static KUrl getSaveUrl(const KUrl &startDir = KUrl(), const QString &filter = QString(),
                           QWidget *parent = 0, const QString &caption = QString());
    static KUrl getSaveUrl(const KUrl &startDir, const QString &filter, QWidget *parent,
                           const QString &caption, Options options);
    static QString getExistingDirectory(const KUrl &startDir = KUrl(), QWidget *parent = 0,
                                        const QString &caption = QString());
    static KUrl getExistingDirectoryUrl(const KUrl &startDir = KUrl(), QWidget *parent = 0,
                                        const QString &caption = QString());
    static KUrl getImageOpenUrl(const KUrl &startDir = KUrl(), QWidget *parent = 0,
                                const QString &caption = QString());

    /* helpers shared with KEncodingFileDialog */
    static KUrl getStartUrl(const KUrl &startDir, QString &recentDirClass);
    static void setStartDir(const KUrl &directory);
    static QString qtFilter(const QString &kdeFilter);
    static void rememberDir(const QString &recentDirClass, const QString &path);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KFileDialog::Options)

#endif
