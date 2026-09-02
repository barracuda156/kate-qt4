/* qt4compat: kstandardshortcut.h (kdelibs 4.14 API) */
#ifndef KDECOMPAT_KSTANDARDSHORTCUT_H
#define KDECOMPAT_KSTANDARDSHORTCUT_H

#include <QtCore/QString>
#include <kdemacros.h>
#include <kshortcut.h>

namespace KStandardShortcut
{
    enum StandardShortcut {
        AccelNone = 0,
        Open, New, Close, Save,
        Print,
        Quit,
        Undo, Redo, Cut, Copy, Paste, PasteSelection,
        SelectAll, Deselect, DeleteWordBack, DeleteWordForward,
        Find, FindNext, FindPrev, Replace,
        Home, Begin, End, Prior, Next,
        Up, Back, Forward, Reload,
        BeginningOfLine, EndOfLine, GotoLine,
        BackwardWord, ForwardWord,
        AddBookmark, ZoomIn, ZoomOut, FullScreen,
        ShowMenubar,
        TabNext, TabPrev,
        Help, WhatsThis,
        TextCompletion, PrevCompletion, NextCompletion, SubstringCompletion,
        RotateUp, RotateDown,
        OpenRecent,
        SaveAs,
        Revert,
        PrintPreview,
        Mail,
        Clear,
        ActualSize,
        FitToPage,
        FitToWidth,
        FitToHeight,
        Zoom,
        Goto,
        GotoPage,
        DocumentBack,
        DocumentForward,
        EditBookmarks,
        Spelling,
        ShowToolbar,
        ShowStatusbar,
        SaveOptions,
        KeyBindings,
        Preferences,
        ConfigureToolbars,
        ConfigureNotifications,
        TipofDay,
        ReportBug,
        SwitchApplicationLanguage,
        AboutApp,
        AboutKDE,
        StandardShortcutCount
    };

    KDEUI_EXPORT const KShortcut &shortcut(StandardShortcut id);
    KDEUI_EXPORT QString name(StandardShortcut id);
    KDEUI_EXPORT QString label(StandardShortcut id);
    KDEUI_EXPORT QString whatsThis(StandardShortcut id);
    KDEUI_EXPORT StandardShortcut find(const QKeySequence &keySeq);
    KDEUI_EXPORT StandardShortcut find(const char *keyName);
    KDEUI_EXPORT KShortcut hardcodedDefaultShortcut(StandardShortcut id);
    KDEUI_EXPORT void saveShortcut(StandardShortcut id, const KShortcut &newShortcut);

    KDEUI_EXPORT const KShortcut &open();
    KDEUI_EXPORT const KShortcut &openNew();
    KDEUI_EXPORT const KShortcut &close();
    KDEUI_EXPORT const KShortcut &save();
    KDEUI_EXPORT const KShortcut &print();
    KDEUI_EXPORT const KShortcut &quit();
    KDEUI_EXPORT const KShortcut &undo();
    KDEUI_EXPORT const KShortcut &redo();
    KDEUI_EXPORT const KShortcut &cut();
    KDEUI_EXPORT const KShortcut &copy();
    KDEUI_EXPORT const KShortcut &paste();
    KDEUI_EXPORT const KShortcut &pasteSelection();
    KDEUI_EXPORT const KShortcut &selectAll();
    KDEUI_EXPORT const KShortcut &deleteWordBack();
    KDEUI_EXPORT const KShortcut &deleteWordForward();
    KDEUI_EXPORT const KShortcut &find();
    KDEUI_EXPORT const KShortcut &findNext();
    KDEUI_EXPORT const KShortcut &findPrev();
    KDEUI_EXPORT const KShortcut &replace();
    KDEUI_EXPORT const KShortcut &zoomIn();
    KDEUI_EXPORT const KShortcut &zoomOut();
    KDEUI_EXPORT const KShortcut &insert();
    KDEUI_EXPORT const KShortcut &home();
    KDEUI_EXPORT const KShortcut &begin();
    KDEUI_EXPORT const KShortcut &end();
    KDEUI_EXPORT const KShortcut &beginningOfLine();
    KDEUI_EXPORT const KShortcut &endOfLine();
    KDEUI_EXPORT const KShortcut &prior();
    KDEUI_EXPORT const KShortcut &next();
    KDEUI_EXPORT const KShortcut &gotoLine();
    KDEUI_EXPORT const KShortcut &addBookmark();
    KDEUI_EXPORT const KShortcut &tabNext();
    KDEUI_EXPORT const KShortcut &tabPrev();
    KDEUI_EXPORT const KShortcut &fullScreen();
    KDEUI_EXPORT const KShortcut &help();
    KDEUI_EXPORT const KShortcut &completion();
    KDEUI_EXPORT const KShortcut &prevCompletion();
    KDEUI_EXPORT const KShortcut &nextCompletion();
    KDEUI_EXPORT const KShortcut &substringCompletion();
    KDEUI_EXPORT const KShortcut &rotateUp();
    KDEUI_EXPORT const KShortcut &rotateDown();
    KDEUI_EXPORT const KShortcut &whatsThis();
    KDEUI_EXPORT const KShortcut &reload();
    KDEUI_EXPORT const KShortcut &up();
    KDEUI_EXPORT const KShortcut &back();
    KDEUI_EXPORT const KShortcut &forward();
    KDEUI_EXPORT const KShortcut &backwardWord();
    KDEUI_EXPORT const KShortcut &forwardWord();
    KDEUI_EXPORT const KShortcut &showMenubar();
}

#endif
