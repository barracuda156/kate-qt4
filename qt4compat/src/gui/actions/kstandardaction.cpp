/* qt4compat: kstandardaction.cpp - after kdelibs 4.14 kdeui/actions/kstandardaction.cpp
 * (LGPL 2, (C) 1999,2000 Kurt Granroth, 2001,2002 Ellis Whitehead) */
#include <kstandardaction.h>
#include <kstandardaction_p.h>

#include <kaboutdata.h>
#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <kicon.h>
#include <klocale.h>
#include <kpastetextaction.h>
#include <krecentfilesaction.h>
#include <kstandardshortcut.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>

#include <QtGui/QApplication>

namespace KStandardAction
{

AutomaticAction::AutomaticAction(const KIcon &icon, const QString &text, const KShortcut &shortcut, const char *slot, QObject *parent)
    : KAction(parent)
{
    setIcon(icon);
    setText(text);
    setShortcut(shortcut);
    connect(this, SIGNAL(triggered()), this, slot);
}

QStringList stdNames()
{
    return internal_stdNames();
}

QList<StandardAction> actionIds()
{
    QList<StandardAction> result;
    for (uint i = 0; g_rgActionInfo[i].id != ActionNone; i++)
        result.append(g_rgActionInfo[i].id);
    return result;
}

KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id)
{
    const KStandardActionInfo *pInfo = infoPtr(id);
    return (pInfo) ? pInfo->idAccel : KStandardShortcut::AccelNone;
}

KAction *create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent)
{
    KAction *pAction = 0;
    const KStandardActionInfo *pInfo = infoPtr(id);
    if (!pInfo)
        return 0;

    QString sLabel;
    QString iconName = QString::fromLatin1(pInfo->psIconName);
    switch (id) {
    case Back:
        sLabel = i18nc("go back", "&Back");
        if (QApplication::isRightToLeft())
            iconName = QLatin1String("go-next");
        break;
    case Forward:
        sLabel = i18nc("go forward", "&Forward");
        if (QApplication::isRightToLeft())
            iconName = QLatin1String("go-previous");
        break;
    case Home:
        sLabel = i18nc("home page", "&Home");
        break;
    case Help:
        sLabel = i18nc("show help", "&Help");
        break;
    case Preferences:
    case AboutApp:
    case HelpContents: {
        const KAboutData *aboutData = KGlobal::mainComponent().aboutData();
        QString appName = aboutData ? aboutData->programName() : QCoreApplication::applicationName();
        sLabel = i18n(pInfo->psLabel, appName);
        break;
    }
    default:
        sLabel = pInfo->psLabel ? i18n(pInfo->psLabel) : QString();
    }

    if (QApplication::isRightToLeft()) {
        switch (id) {
        case Prior: iconName = QLatin1String("go-next-view-page"); break;
        case Next: iconName = QLatin1String("go-previous-view-page"); break;
        case FirstPage: iconName = QLatin1String("go-last-view-page"); break;
        case LastPage: iconName = QLatin1String("go-first-view-page"); break;
        case DocumentBack: iconName = QLatin1String("go-next"); break;
        case DocumentForward: iconName = QLatin1String("go-previous"); break;
        default: break;
        }
    }

    KIcon icon = iconName.isEmpty() ? KIcon() : KIcon(iconName);

    switch (id) {
    case OpenRecent:
        pAction = new KRecentFilesAction(parent);
        break;
    case ShowMenubar:
    case ShowToolbar:
    case ShowStatusbar: {
        KToggleAction *ret = new KToggleAction(parent);
        ret->setChecked(true);
        pAction = ret;
        break;
    }
    case FullScreen: {
        KToggleFullScreenAction *ret = new KToggleFullScreenAction(parent);
        ret->setChecked(false);
        pAction = ret;
        break;
    }
    case PasteText:
        pAction = new KPasteTextAction(parent);
        break;
    case Cut:
    case Copy:
    case Paste:
    case Clear:
    case SelectAll:
        if (!recvr && !slot) {
            const char *autoSlot = 0;
            switch (id) {
            case Cut: autoSlot = SLOT(cut()); break;
            case Copy: autoSlot = SLOT(copy()); break;
            case Paste: autoSlot = SLOT(paste()); break;
            case Clear: autoSlot = SLOT(clear()); break;
            default: autoSlot = SLOT(selectAll()); break;
            }
            pAction = new AutomaticAction(icon, sLabel, KStandardShortcut::shortcut(pInfo->idAccel), autoSlot, parent);
        } else {
            pAction = new KAction(parent);
        }
        break;
    default:
        pAction = new KAction(parent);
        break;
    }

    pAction->setText(sLabel);
    if (pInfo->psToolTip)
        pAction->setHelpText(i18n(pInfo->psToolTip));
    pAction->setIcon(icon);

    KShortcut cut = KStandardShortcut::shortcut(pInfo->idAccel);
    if (!cut.isEmpty())
        pAction->setShortcut(cut);

    pAction->setObjectName(QString::fromLatin1(pInfo->psName));

    // macOS moves actions into the application menu by their text unless
    // told otherwise ("Configure Shortcuts..." would become "Preferences...");
    // KAction defaults to NoRole, so say which standard actions really belong
    // there - as kdelibs does
    switch (id) {
    case Preferences:
        pAction->setMenuRole(QAction::PreferencesRole);
        break;
    case Quit:
        pAction->setMenuRole(QAction::QuitRole);
        break;
    case AboutApp:
        pAction->setMenuRole(QAction::AboutRole);
        break;
    default:
        pAction->setMenuRole(QAction::NoRole);
        break;
    }

    if (recvr && slot) {
        if (id == OpenRecent)
            QObject::connect(pAction, SIGNAL(urlSelected(KUrl)), recvr, slot);
        else if (id == ConfigureToolbars)
            QObject::connect(pAction, SIGNAL(triggered(bool)), recvr, slot, Qt::QueuedConnection);
        else
            QObject::connect(pAction, SIGNAL(triggered(bool)), recvr, slot);
    }

    KActionCollection *collection = qobject_cast<KActionCollection *>(parent);
    if (collection)
        collection->addAction(pAction->objectName(), pAction);

    return pAction;
}

const char *name(StandardAction id)
{
    const KStandardActionInfo *pInfo = infoPtr(id);
    return (pInfo) ? pInfo->psName : 0;
}

KAction *openNew(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(New, recvr, slot, parent); }
KAction *open(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Open, recvr, slot, parent); }
KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent)
{ return static_cast<KRecentFilesAction *>(KStandardAction::create(OpenRecent, recvr, slot, parent)); }
KAction *save(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Save, recvr, slot, parent); }
KAction *saveAs(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(SaveAs, recvr, slot, parent); }
KAction *revert(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Revert, recvr, slot, parent); }
KAction *print(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Print, recvr, slot, parent); }
KAction *printPreview(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(PrintPreview, recvr, slot, parent); }
KAction *close(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Close, recvr, slot, parent); }
KAction *mail(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Mail, recvr, slot, parent); }
KAction *quit(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Quit, recvr, slot, parent); }
KAction *undo(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Undo, recvr, slot, parent); }
KAction *redo(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Redo, recvr, slot, parent); }
KAction *cut(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Cut, recvr, slot, parent); }
KAction *copy(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Copy, recvr, slot, parent); }
KAction *paste(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Paste, recvr, slot, parent); }
KAction *pasteText(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(PasteText, recvr, slot, parent); }
KAction *clear(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Clear, recvr, slot, parent); }
KAction *selectAll(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(SelectAll, recvr, slot, parent); }
KAction *deselect(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Deselect, recvr, slot, parent); }
KAction *find(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Find, recvr, slot, parent); }
KAction *findNext(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FindNext, recvr, slot, parent); }
KAction *findPrev(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FindPrev, recvr, slot, parent); }
KAction *replace(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Replace, recvr, slot, parent); }
KAction *actualSize(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ActualSize, recvr, slot, parent); }
KAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FitToPage, recvr, slot, parent); }
KAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FitToWidth, recvr, slot, parent); }
KAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FitToHeight, recvr, slot, parent); }
KAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ZoomIn, recvr, slot, parent); }
KAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ZoomOut, recvr, slot, parent); }
KAction *zoom(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Zoom, recvr, slot, parent); }
KAction *redisplay(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Redisplay, recvr, slot, parent); }
KAction *up(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Up, recvr, slot, parent); }
KAction *back(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Back, recvr, slot, parent); }
KAction *forward(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Forward, recvr, slot, parent); }
KAction *home(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Home, recvr, slot, parent); }
KAction *prior(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Prior, recvr, slot, parent); }
KAction *next(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Next, recvr, slot, parent); }
KAction *goTo(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Goto, recvr, slot, parent); }
KAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(GotoPage, recvr, slot, parent); }
KAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(GotoLine, recvr, slot, parent); }
KAction *firstPage(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(FirstPage, recvr, slot, parent); }
KAction *lastPage(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(LastPage, recvr, slot, parent); }
KAction *documentBack(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(DocumentBack, recvr, slot, parent); }
KAction *documentForward(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(DocumentForward, recvr, slot, parent); }
KAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(AddBookmark, recvr, slot, parent); }
KAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(EditBookmarks, recvr, slot, parent); }
KAction *spelling(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Spelling, recvr, slot, parent); }

static KToggleAction *buildAutomaticToggleAction(StandardAction id, const QObject *recvr, const char *slot, QObject *parent)
{
    KToggleAction *action = static_cast<KToggleAction *>(KStandardAction::create(id, recvr, slot, parent));
    action->setChecked(true);
    return action;
}

KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent)
{
    KToggleAction *ret = buildAutomaticToggleAction(ShowMenubar, recvr, slot, parent);
    ret->setWhatsThis(i18n("Show Menubar<p>Shows the menubar again after it has been hidden</p>"));
    return ret;
}

KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent)
{
    KToggleAction *ret = buildAutomaticToggleAction(ShowStatusbar, recvr, slot, parent);
    ret->setWhatsThis(i18n("Show Statusbar<p>Shows the statusbar, which is the bar at the bottom of the window used for status information.</p>"));
    return ret;
}

KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent)
{
    KToggleFullScreenAction *ret = static_cast<KToggleFullScreenAction *>(KStandardAction::create(FullScreen, recvr, slot, parent));
    ret->setWindow(window);
    return ret;
}

KAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(SaveOptions, recvr, slot, parent); }
KAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(KeyBindings, recvr, slot, parent); }
KAction *preferences(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Preferences, recvr, slot, parent); }
KAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ConfigureToolbars, recvr, slot, parent); }
KAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ConfigureNotifications, recvr, slot, parent); }
KAction *help(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(Help, recvr, slot, parent); }
KAction *helpContents(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(HelpContents, recvr, slot, parent); }
KAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(WhatsThis, recvr, slot, parent); }
KAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(TipofDay, recvr, slot, parent); }
KAction *reportBug(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(ReportBug, recvr, slot, parent); }
KAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(AboutApp, recvr, slot, parent); }
KAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent) { return KStandardAction::create(AboutKDE, recvr, slot, parent); }

KAction *cut(QObject *parent) { return KStandardAction::create(Cut, 0, 0, parent); }
KAction *copy(QObject *parent) { return KStandardAction::create(Copy, 0, 0, parent); }
KAction *paste(QObject *parent) { return KStandardAction::create(Paste, 0, 0, parent); }
KAction *clear(QObject *parent) { return KStandardAction::create(Clear, 0, 0, parent); }
KAction *selectAll(QObject *parent) { return KStandardAction::create(SelectAll, 0, 0, parent); }

}
