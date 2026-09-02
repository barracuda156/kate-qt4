/* qt4compat: kstandardaction.h (kdelibs 4.14 API) */
#ifndef KDECOMPAT_KSTANDARDACTION_H
#define KDECOMPAT_KSTANDARDACTION_H

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <kstandardshortcut.h>

class QObject;
class QWidget;
class KAction;
class KRecentFilesAction;
class KToggleAction;
class KToggleFullScreenAction;

namespace KStandardAction
{
    enum StandardAction {
        ActionNone,
        New, Open, OpenRecent, Save, SaveAs, Revert, Close,
        Print, PrintPreview, Mail, Quit,
        Undo, Redo, Cut, Copy, Paste, SelectAll, Deselect, Find, FindNext, FindPrev,
        Replace,
        ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut,
        Zoom, Redisplay,
        Up, Back, Forward, Home, Prior, Next, Goto, GotoPage, GotoLine,
        FirstPage, LastPage, DocumentBack, DocumentForward,
        AddBookmark, EditBookmarks,
        Spelling,
        ShowMenubar, ShowToolbar, ShowStatusbar,
        SaveOptions, KeyBindings,
        Preferences, ConfigureToolbars,
        Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
        TipofDay,
        ConfigureNotifications,
        FullScreen,
        Clear,
        PasteText,
        SwitchApplicationLanguage
    };

    KDEUI_EXPORT KAction *create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT const char *name(StandardAction id);
    inline const char *stdName(StandardAction act_enum) { return name(act_enum); }
    KDEUI_EXPORT QStringList stdNames();
    KDEUI_EXPORT QList<StandardAction> actionIds();
    KDEUI_EXPORT KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);

    KDEUI_EXPORT KAction *openNew(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *open(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *save(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *revert(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *close(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *print(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *mail(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *quit(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *undo(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *redo(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *cut(QObject *parent);
    KDEUI_EXPORT KAction *copy(QObject *parent);
    KDEUI_EXPORT KAction *paste(QObject *parent);
    KDEUI_EXPORT KAction *clear(QObject *parent);
    KDEUI_EXPORT KAction *selectAll(QObject *parent);
    KDEUI_EXPORT KAction *cut(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *copy(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *paste(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *pasteText(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *clear(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *deselect(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *find(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *findNext(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *replace(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *zoom(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *up(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *back(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *forward(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *home(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *prior(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *next(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *goTo(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *spelling(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent);
    KDEUI_EXPORT KAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *preferences(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *help(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *helpContents(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *reportBug(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent);
    KDEUI_EXPORT KAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent);
}

#endif
