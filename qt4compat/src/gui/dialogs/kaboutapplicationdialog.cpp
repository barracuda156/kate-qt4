/* qt4compat: kaboutapplicationdialog.cpp - tabbed about dialog built from KAboutData */
#include <kaboutapplicationdialog.h>

#include <kaboutdata.h>
#include <kglobal.h>
#include <kicon.h>
#include <klocale.h>

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

class KAboutApplicationDialog::Private
{
public:
    void init(KAboutApplicationDialog *dlg, const KAboutData *aboutData);
    QString peopleHtml(const QList<KAboutPerson> &people) const
    {
        QString html = QLatin1String("<qt><ul style=\"margin-left:0px; padding-left:0px; list-style:none;\">");
        foreach (const KAboutPerson &p, people) {
            html += QLatin1String("<li style=\"margin-bottom:6px;\"><b>") + Qt::escape(p.name()) + QLatin1String("</b>");
            if (!p.task().isEmpty())
                html += QLatin1String("<br/>") + Qt::escape(p.task());
            if (!p.emailAddress().isEmpty())
                html += QLatin1String("<br/><a href=\"mailto:") + p.emailAddress() + QLatin1String("\">") + Qt::escape(p.emailAddress()) + QLatin1String("</a>");
            if (!p.webAddress().isEmpty())
                html += QLatin1String("<br/><a href=\"") + p.webAddress() + QLatin1String("\">") + Qt::escape(p.webAddress()) + QLatin1String("</a>");
            html += QLatin1String("</li>");
        }
        html += QLatin1String("</ul></qt>");
        return html;
    }
    QTextBrowser *browser(const QString &html, QWidget *parent) const
    {
        QTextBrowser *b = new QTextBrowser(parent);
        b->setFrameStyle(QFrame::NoFrame);
        b->setOpenExternalLinks(true);
        b->setHtml(html);
        return b;
    }
};

void KAboutApplicationDialog::Private::init(KAboutApplicationDialog *dlg, const KAboutData *aboutData)
{
    if (!aboutData) {
        dlg->setCaption(i18n("About"));
        dlg->setButtons(Close);
        dlg->setMainWidget(new QLabel(i18n("No information available."), dlg));
        return;
    }
    dlg->setCaption(i18n("About %1", aboutData->programName()));
    dlg->setButtons(Close);
    dlg->setDefaultButton(Close);
    dlg->setModal(false);

    QWidget *main = new QWidget(dlg);
    QVBoxLayout *layout = new QVBoxLayout(main);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel(main);
    QIcon icon = KIcon(aboutData->programIconName());
    if (icon.isNull())
        icon = QApplication::windowIcon();
    iconLabel->setPixmap(icon.pixmap(48, 48));
    titleLayout->addWidget(iconLabel);
    QLabel *titleLabel = new QLabel(main);
    titleLabel->setText(QString::fromLatin1("<qt><font size=\"5\">%1</font><br/>%2 %3<br/>%4</qt>")
                        .arg(Qt::escape(aboutData->programName()))
                        .arg(i18n("Version"))
                        .arg(Qt::escape(aboutData->version()))
                        .arg(i18n("Using Qt %1", QString::fromLatin1(qVersion()))));
    titleLayout->addWidget(titleLabel, 1);
    layout->addLayout(titleLayout);

    QTabWidget *tabs = new QTabWidget(main);
    layout->addWidget(tabs, 1);

    QString about = QLatin1String("<qt>");
    if (!aboutData->shortDescription().isEmpty())
        about += QLatin1String("<p>") + Qt::escape(aboutData->shortDescription()) + QLatin1String("</p>");
    if (!aboutData->otherText().isEmpty())
        about += QLatin1String("<p>") + Qt::escape(aboutData->otherText()) + QLatin1String("</p>");
    if (!aboutData->copyrightStatement().isEmpty())
        about += QLatin1String("<p>") + Qt::escape(aboutData->copyrightStatement()) + QLatin1String("</p>");
    if (!aboutData->homepage().isEmpty())
        about += QLatin1String("<p><a href=\"") + aboutData->homepage() + QLatin1String("\">") + Qt::escape(aboutData->homepage()) + QLatin1String("</a></p>");
    if (!aboutData->licenseName(KAboutData::ShortName).isEmpty())
        about += QLatin1String("<p>") + i18n("License: %1", Qt::escape(aboutData->licenseName(KAboutData::FullName))) + QLatin1String("</p>");
    about += QLatin1String("</qt>");
    tabs->addTab(browser(about, tabs), i18n("&About"));

    if (!aboutData->authors().isEmpty())
        tabs->addTab(browser(peopleHtml(aboutData->authors()), tabs), i18n("A&uthors"));
    if (!aboutData->credits().isEmpty())
        tabs->addTab(browser(peopleHtml(aboutData->credits()), tabs), i18n("&Thanks To"));
    if (!aboutData->translators().isEmpty())
        tabs->addTab(browser(peopleHtml(aboutData->translators()), tabs), i18n("T&ranslation"));
    if (aboutData->licenseInfo().key() != KAboutData::License_Unknown) {
        QTextBrowser *lic = browser(QString(), tabs);
        lic->setPlainText(aboutData->license());
        tabs->addTab(lic, i18n("&License Agreement"));
    }

    dlg->setMainWidget(main);
    dlg->setInitialSize(QSize(520, 420));
}

KAboutApplicationDialog::KAboutApplicationDialog(const KAboutData *aboutData, Options opts, QWidget *parent)
    : KDialog(parent), d(new Private)
{
    Q_UNUSED(opts)
    d->init(this, aboutData);
}

KAboutApplicationDialog::KAboutApplicationDialog(const KAboutData *aboutData, QWidget *parent)
    : KDialog(parent), d(new Private)
{
    d->init(this, aboutData);
}

KAboutApplicationDialog::~KAboutApplicationDialog()
{
    delete d;
}
