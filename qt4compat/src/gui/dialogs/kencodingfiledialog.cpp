/* qt4compat: kencodingfiledialog.cpp */
#include <kencodingfiledialog.h>

#include <kcharsets.h>
#include <kglobal.h>
#include <klocale.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtGui/QComboBox>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

KEncodingFileDialog::KEncodingFileDialog(const QString &startDir, const QString &encoding, const QString &filter,
                                         const QString &caption, KFileDialog::OperationMode type, QWidget *parent)
    : QDialog(parent), m_fileDialog(0), m_encodingCombo(0)
{
    setWindowTitle(caption.isEmpty() ? (type == KFileDialog::Saving ? i18n("Save As") : i18n("Open")) : caption);

    KUrl start;
    QString suggestedName;
    if (!startDir.isEmpty()) {
        if (type == KFileDialog::Saving && !QFileInfo(startDir).isDir() && !startDir.startsWith(QLatin1Char(':'))
            && !startDir.startsWith(QLatin1String("kfiledialog:"))) {
            const QFileInfo fi(startDir.startsWith(QLatin1String("file:")) ? KUrl(startDir).toLocalFile() : startDir);
            suggestedName = fi.fileName();
            if (!fi.absolutePath().isEmpty() && QDir::isAbsolutePath(startDir))
                start = KUrl::fromPath(fi.absolutePath());
        } else {
            start = KUrl(startDir);
        }
    }
    const KUrl startUrl = KFileDialog::getStartUrl(start, m_recentDirClass);
    QString dir = startUrl.toLocalFile();
    if (dir.isEmpty())
        dir = QDir::homePath();
    if (QFileInfo(dir).isFile()) {
        if (suggestedName.isEmpty())
            suggestedName = QFileInfo(dir).fileName();
        dir = QFileInfo(dir).absolutePath();
    }

    m_fileDialog = new QFileDialog(this, QString(), dir, KFileDialog::qtFilter(filter));
    m_fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    m_fileDialog->setWindowFlags(Qt::Widget);
    m_fileDialog->setSizeGripEnabled(false);
    m_fileDialog->setAcceptMode(type == KFileDialog::Saving ? QFileDialog::AcceptSave : QFileDialog::AcceptOpen);
    m_fileDialog->setFileMode(type == KFileDialog::Saving ? QFileDialog::AnyFile : QFileDialog::ExistingFile);
    if (type == KFileDialog::Saving)
        m_fileDialog->setConfirmOverwrite(true);
    if (!suggestedName.isEmpty())
        m_fileDialog->selectFile(suggestedName);
    connect(m_fileDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(slotFilesSelected(QStringList)));
    connect(m_fileDialog, SIGNAL(rejected()), this, SLOT(slotFileDialogRejected()));

    QWidget *encodingRow = new QWidget(this);
    QHBoxLayout *hl = new QHBoxLayout(encodingRow);
    hl->setContentsMargins(0, 0, 0, 0);
    QLabel *label = new QLabel(i18n("Encoding:"), encodingRow);
    m_encodingCombo = new QComboBox(encodingRow);
    label->setBuddy(m_encodingCombo);
    hl->addWidget(label);
    hl->addWidget(m_encodingCombo, 1);

    const QStringList encodings = KGlobal::charsets()->descriptiveEncodingNames();
    int insert = 0;
    int current = -1;
    QString wanted = encoding;
    if (wanted.isEmpty())
        wanted = QString::fromLatin1(QTextCodec::codecForLocale()->name());
    QTextCodec *wantedCodec = QTextCodec::codecForName(wanted.toLatin1());
    foreach (const QString &desc, encodings) {
        const QString enc = KGlobal::charsets()->encodingForName(desc);
        if (enc.isEmpty())
            continue;
        m_encodingCombo->addItem(desc, enc);
        QTextCodec *c = QTextCodec::codecForName(enc.toLatin1());
        if (current < 0 && ((wantedCodec && c == wantedCodec) || enc.compare(wanted, Qt::CaseInsensitive) == 0))
            current = insert;
        ++insert;
    }
    if (current >= 0)
        m_encodingCombo->setCurrentIndex(current);

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(m_fileDialog, 1);
    vl->addWidget(encodingRow);
    resize(m_fileDialog->sizeHint().expandedTo(QSize(640, 480)) + QSize(0, encodingRow->sizeHint().height()));
}

KEncodingFileDialog::~KEncodingFileDialog()
{
}

QString KEncodingFileDialog::selectedEncoding() const
{
    if (m_encodingCombo->currentIndex() < 0)
        return QString();
    return m_encodingCombo->itemData(m_encodingCombo->currentIndex()).toString();
}

void KEncodingFileDialog::setMultiSelection(bool multi)
{
    if (m_fileDialog->acceptMode() == QFileDialog::AcceptOpen)
        m_fileDialog->setFileMode(multi ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
}

QStringList KEncodingFileDialog::selectedFiles() const
{
    return m_selected;
}

QString KEncodingFileDialog::recentDirClass() const
{
    return m_recentDirClass;
}

void KEncodingFileDialog::slotFilesSelected(const QStringList &files)
{
    m_selected = files;
    if (!files.isEmpty())
        KFileDialog::rememberDir(m_recentDirClass, files.first());
    accept();
}

void KEncodingFileDialog::slotFileDialogRejected()
{
    reject();
}

static KEncodingFileDialog::Result runDialog(const QString &encoding, const QString &startDir, const QString &filter,
                                             QWidget *parent, const QString &caption, KFileDialog::OperationMode mode, bool multi)
{
    KEncodingFileDialog dlg(startDir, encoding, filter, caption, mode, parent);
    dlg.setMultiSelection(multi);
    KEncodingFileDialog::Result res;
    if (dlg.exec() == QDialog::Accepted) {
        res.fileNames = dlg.selectedFiles();
        foreach (const QString &f, res.fileNames)
            res.URLs.append(KUrl::fromPath(f));
        res.encoding = dlg.selectedEncoding();
    }
    return res;
}

KEncodingFileDialog::Result KEncodingFileDialog::getOpenFileNameAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Opening, false);
}

KEncodingFileDialog::Result KEncodingFileDialog::getOpenFileNamesAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Opening, true);
}

KEncodingFileDialog::Result KEncodingFileDialog::getOpenUrlAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Opening, false);
}

KEncodingFileDialog::Result KEncodingFileDialog::getOpenUrlsAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Opening, true);
}

KEncodingFileDialog::Result KEncodingFileDialog::getSaveFileNameAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Saving, false);
}

KEncodingFileDialog::Result KEncodingFileDialog::getSaveUrlAndEncoding(const QString &encoding, const QString &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return runDialog(encoding, startDir, filter, parent, caption, KFileDialog::Saving, false);
}
