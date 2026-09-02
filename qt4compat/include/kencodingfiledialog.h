/* qt4compat: kencodingfiledialog.h - file dialog with an encoding selector.
 * A QDialog embedding a non-native QFileDialog widget and a KCharsets driven combo box. */
#ifndef KDECOMPAT_KENCODINGFILEDIALOG_H
#define KDECOMPAT_KENCODINGFILEDIALOG_H

#include <QtCore/QStringList>
#include <QtGui/QDialog>
#include <kdemacros.h>
#include <kfiledialog.h>
#include <kurl.h>

class QComboBox;
class QFileDialog;

class KDEUI_EXPORT KEncodingFileDialog : public QDialog
{
    Q_OBJECT
public:
    class Result
    {
    public:
        QStringList fileNames;
        KUrl::List URLs;
        QString encoding;
    };

    KEncodingFileDialog(const QString &startDir = QString(), const QString &encoding = QString(),
                        const QString &filter = QString(), const QString &caption = QString(),
                        KFileDialog::OperationMode type = KFileDialog::Opening, QWidget *parent = 0);
    ~KEncodingFileDialog();

    QString selectedEncoding() const;
    void setMultiSelection(bool multi);
    QStringList selectedFiles() const;
    QString recentDirClass() const;

    static Result getOpenFileNameAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                             const QString &filter = QString(), QWidget *parent = 0,
                                             const QString &caption = QString());
    static Result getOpenFileNamesAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                              const QString &filter = QString(), QWidget *parent = 0,
                                              const QString &caption = QString());
    static Result getOpenUrlAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                        const QString &filter = QString(), QWidget *parent = 0,
                                        const QString &caption = QString());
    static Result getOpenUrlsAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                         const QString &filter = QString(), QWidget *parent = 0,
                                         const QString &caption = QString());
    static Result getSaveFileNameAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                             const QString &filter = QString(), QWidget *parent = 0,
                                             const QString &caption = QString());
    static Result getSaveUrlAndEncoding(const QString &encoding = QString(), const QString &startDir = QString(),
                                        const QString &filter = QString(), QWidget *parent = 0,
                                        const QString &caption = QString());

private Q_SLOTS:
    void slotFilesSelected(const QStringList &files);
    void slotFileDialogRejected();

private:
    QFileDialog *m_fileDialog;
    QComboBox *m_encodingCombo;
    QStringList m_selected;
    QString m_recentDirClass;
};

#endif
