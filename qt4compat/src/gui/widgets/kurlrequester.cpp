/* qt4compat: kurlrequester.cpp - see kurlrequester.h */
#include "kurlrequester.h"

#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kicon.h>
#include <kfiledialog.h>
#include <klocale.h>

#include <QtGui/QHBoxLayout>

KUrlRequester::KUrlRequester(QWidget *parent)
    : QFrame(parent), m_edit(0), m_button(0), m_mode(KFile::File | KFile::LocalOnly)
{
    init(false);
}

KUrlRequester::KUrlRequester(const KUrl &url, QWidget *parent)
    : QFrame(parent), m_edit(0), m_button(0), m_mode(KFile::File | KFile::LocalOnly)
{
    init(false);
    setUrl(url);
}

KUrlRequester::KUrlRequester(bool useComboBox, QWidget *parent)
    : QFrame(parent), m_edit(0), m_button(0), m_mode(KFile::File | KFile::LocalOnly)
{
    init(useComboBox);
}

KUrlRequester::~KUrlRequester()
{
}

void KUrlRequester::init(bool useComboBox)
{
    setFrameStyle(QFrame::NoFrame);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(4);

    if (useComboBox) {
        KComboBox *combo = new KComboBox(true, this);
        m_edit = combo;
        connect(combo, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
        connect(combo, SIGNAL(returnPressed(QString)), this, SIGNAL(returnPressed(QString)));
        connect(combo, SIGNAL(editTextChanged(QString)), this, SIGNAL(textChanged(QString)));
    } else {
        KLineEdit *edit = new KLineEdit(this);
        m_edit = edit;
        connect(edit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
        connect(edit, SIGNAL(returnPressed(QString)), this, SIGNAL(returnPressed(QString)));
        connect(edit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
    }
    layout->addWidget(m_edit);

    m_button = new KPushButton(this);
    m_button->setIcon(KIcon("folder-open"));
    m_button->setToolTip(i18n("Open file dialog"));
    m_button->setFixedWidth(m_button->sizeHint().height());
    layout->addWidget(m_button);
    connect(m_button, SIGNAL(clicked()), this, SLOT(slotOpenDialog()));

    setFocusProxy(m_edit);
}

void KUrlRequester::setUrl(const KUrl &url)
{
    setText(url.pathOrUrl());
}

KUrl KUrlRequester::url() const
{
    return KUrl(text());
}

void KUrlRequester::setText(const QString &text)
{
    if (KComboBox *combo = qobject_cast<KComboBox *>(m_edit)) {
        combo->setEditText(text);
    } else if (KLineEdit *edit = qobject_cast<KLineEdit *>(m_edit)) {
        edit->setText(text);
    }
}

QString KUrlRequester::text() const
{
    if (KComboBox *combo = qobject_cast<KComboBox *>(m_edit)) {
        return combo->currentText();
    }
    if (KLineEdit *edit = qobject_cast<KLineEdit *>(m_edit)) {
        return edit->text();
    }
    return QString();
}

void KUrlRequester::setStartDir(const KUrl &startDir)
{
    m_startDir = startDir;
}

KLineEdit *KUrlRequester::lineEdit() const
{
    if (KComboBox *combo = qobject_cast<KComboBox *>(m_edit)) {
        return qobject_cast<KLineEdit *>(combo->lineEdit());
    }
    return qobject_cast<KLineEdit *>(m_edit);
}

KComboBox *KUrlRequester::comboBox() const
{
    return qobject_cast<KComboBox *>(m_edit);
}

KPushButton *KUrlRequester::button() const
{
    return m_button;
}

KFile::Modes KUrlRequester::mode() const
{
    return m_mode;
}

void KUrlRequester::setMode(KFile::Modes mode)
{
    m_mode = mode;
}

QString KUrlRequester::filter() const
{
    return m_filter;
}

void KUrlRequester::setFilter(const QString &filter)
{
    m_filter = filter;
}

void KUrlRequester::slotOpenDialog()
{
    KUrl start = url();
    if (start.isEmpty()) {
        start = m_startDir;
    }

    if (m_mode.testFlag(KFile::Directory)) {
        const KUrl dir = KFileDialog::getExistingDirectoryUrl(start, this);
        if (!dir.isEmpty()) {
            setUrl(dir);
            emit urlSelected(dir);
        }
    } else {
        const KUrl file = KFileDialog::getOpenUrl(start, m_filter, this);
        if (!file.isEmpty()) {
            setUrl(file);
            emit urlSelected(file);
        }
    }
}

KUrlComboRequester::KUrlComboRequester(QWidget *parent)
    : KUrlRequester(true, parent)
{
}
