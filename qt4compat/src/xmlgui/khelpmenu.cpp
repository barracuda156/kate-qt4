/* qt4compat: khelpmenu.cpp */
#include <khelpmenu.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kglobal.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <ktoolinvocation.h>

#include <QtCore/QUrl>
#include <QtGui/QTextDocument>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>
#include <QtGui/QWhatsThis>
#include <QtGui/QWidget>

class KHelpMenu::Private
{
public:
    Private() : parent(0), aboutData(0), menu(0), helpContents(0), whatsThis(0), aboutApp(0), aboutKDE(0), reportBug(0) {}
    QWidget *parent;
    const KAboutData *aboutData;
    QString aboutAppText;
    KMenu *menu;
    QAction *helpContents;
    QAction *whatsThis;
    QAction *aboutApp;
    QAction *aboutKDE;
    QAction *reportBug;
};

KHelpMenu::KHelpMenu(QWidget *parent, const QString &aboutAppText, bool showWhatsThis)
    : QObject(parent), d(new Private)
{
    d->aboutAppText = aboutAppText;
    init(parent, showWhatsThis, 0);
}

KHelpMenu::KHelpMenu(QWidget *parent, const KAboutData *aboutData, bool showWhatsThis, KActionCollection *actions)
    : QObject(parent), d(new Private)
{
    d->aboutData = aboutData;
    init(parent, showWhatsThis, actions);
}

void KHelpMenu::init(QWidget *parent, bool showWhatsThis, KActionCollection *actions)
{
    d->parent = parent;
    QObject *actionParent = actions ? static_cast<QObject *>(actions) : static_cast<QObject *>(this);
    d->helpContents = KStandardAction::helpContents(this, SLOT(appHelpActivated()), actionParent);
    if (showWhatsThis)
        d->whatsThis = KStandardAction::whatsThis(this, SLOT(contextHelpActivated()), actionParent);
    d->reportBug = KStandardAction::reportBug(this, SLOT(reportBug()), actionParent);
    d->aboutApp = KStandardAction::aboutApp(this, SLOT(aboutApplication()), actionParent);
    d->aboutKDE = KStandardAction::aboutKDE(this, SLOT(aboutKDE()), actionParent);
    d->aboutKDE->setText(i18n("About &Qt"));
    d->aboutKDE->setIcon(QIcon());
}

KHelpMenu::~KHelpMenu()
{
    delete d->menu;
    delete d;
}

KMenu *KHelpMenu::menu()
{
    if (!d->menu) {
        d->menu = new KMenu(i18n("&Help"), d->parent);
        d->menu->addAction(d->helpContents);
        if (d->whatsThis)
            d->menu->addAction(d->whatsThis);
        d->menu->addSeparator();
        d->menu->addAction(d->reportBug);
        d->menu->addSeparator();
        d->menu->addAction(d->aboutApp);
        d->menu->addAction(d->aboutKDE);
    }
    return d->menu;
}

QAction *KHelpMenu::action(MenuId id) const
{
    switch (id) {
    case menuHelpContents: return d->helpContents;
    case menuWhatsThis: return d->whatsThis;
    case menuAboutApp: return d->aboutApp;
    case menuAboutKDE: return d->aboutKDE;
    case menuReportBug: return d->reportBug;
    default: return 0;
    }
}

void KHelpMenu::appHelpActivated()
{
    KToolInvocation::invokeHelp();
}

void KHelpMenu::contextHelpActivated()
{
    QWhatsThis::enterWhatsThisMode();
}

static QString aboutText(const KAboutData *about)
{
    QString text = QLatin1String("<h3>") + Qt::escape(about->programName()) + QLatin1String(" ") + Qt::escape(about->version()) + QLatin1String("</h3>");
    if (!about->shortDescription().isEmpty())
        text += QLatin1String("<p>") + Qt::escape(about->shortDescription()) + QLatin1String("</p>");
    if (!about->copyrightStatement().isEmpty())
        text += QLatin1String("<p>") + Qt::escape(about->copyrightStatement()).replace(QLatin1Char('\n'), QLatin1String("<br/>")) + QLatin1String("</p>");
    if (!about->homepage().isEmpty())
        text += QLatin1String("<p><a href=\"") + about->homepage() + QLatin1String("\">") + about->homepage() + QLatin1String("</a></p>");
    text += QLatin1String("<p>") + Qt::escape(about->licenseName(KAboutData::FullName)) + QLatin1String("</p>");
    const QList<KAboutPerson> authors = about->authors();
    if (!authors.isEmpty()) {
        text += QLatin1String("<p><b>") + i18n("Authors") + QLatin1String("</b><br/>");
        int count = 0;
        foreach (const KAboutPerson &p, authors) {
            if (count++ >= 12) {
                text += QLatin1String("...<br/>");
                break;
            }
            text += Qt::escape(p.name());
            if (!p.task().isEmpty())
                text += QLatin1String(" (") + Qt::escape(p.task()) + QLatin1String(")");
            text += QLatin1String("<br/>");
        }
        text += QLatin1String("</p>");
    }
    if (!about->otherText().isEmpty())
        text += QLatin1String("<p>") + Qt::escape(about->otherText()) + QLatin1String("</p>");
    return text;
}

void KHelpMenu::aboutApplication()
{
    if (!d->aboutAppText.isEmpty()) {
        QMessageBox::about(d->parent, i18n("About %1", QCoreApplication::applicationName()), d->aboutAppText);
        return;
    }
    const KAboutData *about = d->aboutData ? d->aboutData : KGlobal::mainComponent().aboutData();
    if (about) {
        QMessageBox::about(d->parent, i18n("About %1", about->programName()), aboutText(about));
        return;
    }
    emit showAboutApplication();
}

void KHelpMenu::aboutKDE()
{
    QMessageBox::aboutQt(d->parent, i18n("About Qt"));
}

void KHelpMenu::reportBug()
{
    const KAboutData *about = d->aboutData ? d->aboutData : KGlobal::mainComponent().aboutData();
    QString address = about ? about->bugAddress() : QString();
    if (address.isEmpty())
        address = QLatin1String("submit@bugs.kde.org");
    if (address.contains(QLatin1String("://")))
        QDesktopServices::openUrl(QUrl(address));
    else
        KToolInvocation::invokeMailer(address, i18n("Bug report: %1", about ? about->programName() : QCoreApplication::applicationName()));
}

void KHelpMenu::switchApplicationLanguage()
{
}
