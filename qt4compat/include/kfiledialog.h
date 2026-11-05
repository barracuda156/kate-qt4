/* qt4compat: kfiledialog.h - KFileDialog over QFileDialog (local files only):
 * the kdelibs static helpers (native dialogs), and the dialog class itself,
 * a KDialog hosting a non-native QFileDialog the way kdelibs' hosts its
 * KFileWidget. KFile::Directory wins over KFile::File/Files (QFileDialog
 * selects either files or a directory, never both); the custom widget
 * argument is ignored. */
#ifndef KDECOMPAT_KFILEDIALOG_H
#define KDECOMPAT_KFILEDIALOG_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFileDialog>
#include <kdemacros.h>
#include <kdialog.h>
#include <kfile.h>
#include <kurl.h>

class KDEUI_EXPORT KFileDialog : public KDialog
{
    Q_OBJECT
public:
    enum OperationMode { Other = 0, Opening, Saving };
    enum Option { ConfirmOverwrite = 0x01, ShowInlinePreview = 0x02 };
    Q_DECLARE_FLAGS(Options, Option)

    KFileDialog(const KUrl &startDir, const QString &filter, QWidget *parent, QWidget *widget = 0);
    ~KFileDialog();

    void setOperationMode(OperationMode mode);
    OperationMode operationMode() const;
    void setMode(KFile::Modes mode);
    KFile::Modes mode() const;
    void setFilter(const QString &filter);
    void setSelection(const QString &name);
    void setUrl(const KUrl &url, bool clearforward = true);
    KUrl baseUrl() const;
    QString selectedFile() const;
    QStringList selectedFiles() const;
    KUrl selectedUrl() const;
    KUrl::List selectedUrls() const;
    KPushButton *okButton() const;
    KPushButton *cancelButton() const;

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

protected Q_SLOTS:
    virtual void slotButtonClicked(int button);

private Q_SLOTS:
    void slotFilesSelected(const QStringList &files);
    void slotFileDialogRejected();

private:
    void applyMode();

    QFileDialog *m_fileDialog;
    OperationMode m_operationMode;
    KFile::Modes m_mode;
    QStringList m_selected;
    QString m_recentDirClass;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KFileDialog::Options)

#endif
