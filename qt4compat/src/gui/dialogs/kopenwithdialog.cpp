/* qt4compat: kopenwithdialog.cpp */
#include <kopenwithdialog.h>

#include <klocale.h>

#include <QtGui/QLabel>

KOpenWithDialog::KOpenWithDialog(const KUrl::List &, QWidget *parent)
    : KDialog(parent)
{
    init(QString(), QString());
}

KOpenWithDialog::KOpenWithDialog(const KUrl::List &, const QString &text, const QString &value, QWidget *parent)
    : KDialog(parent)
{
    init(text, value);
}

KOpenWithDialog::KOpenWithDialog(const QString &, const QString &value, QWidget *parent)
    : KDialog(parent)
{
    init(QString(), value);
}

KOpenWithDialog::KOpenWithDialog(QWidget *parent)
    : KDialog(parent)
{
    init(QString(), QString());
}

void KOpenWithDialog::init(const QString &, const QString &value)
{
    m_text = value;
    setCaption(i18n("Open With"));
    setButtons(Close);
    QLabel *label = new QLabel(i18n("Choosing an application is not available in this build. "
                                    "Use the system's Open With command from the Finder instead."), this);
    label->setWordWrap(true);
    setMainWidget(label);
}

KOpenWithDialog::~KOpenWithDialog()
{
}

QString KOpenWithDialog::text() const
{
    return m_text;
}

void KOpenWithDialog::hideNoCloseOnExit()
{
}

void KOpenWithDialog::hideRunInTerminal()
{
}

KService::Ptr KOpenWithDialog::service() const
{
    return KService::Ptr();
}

void KOpenWithDialog::setSaveNewApplications(bool)
{
}

void KOpenWithDialog::accept()
{
    KDialog::reject();
}
