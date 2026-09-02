/* qt4compat: kstandardshortcut.cpp - table taken from kdelibs 4.14 kdeui/shortcuts/kstandardshortcut.cpp
 * (LGPL 2+, (C) 1997 Stefan Taferner, 2000 Nicolas Hadacek, 2001,2002 Ellis Whitehead).
 * Where Qt knows a QKeySequence::StandardKey for an entry it is used as the
 * default, which yields the platform's native binding (Command on macOS). */
#include <kstandardshortcut.h>

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksharedconfig.h>

#include <QtGui/QKeySequence>

namespace KStandardShortcut
{

struct KStandardShortcutInfo
{
    StandardShortcut id;
    const char *name;
    const char *translation_context;
    const char *description;
    int cutDefault, cutDefault2;
    QKeySequence::StandardKey stdKey;
    KShortcut cut;
    bool isInitialized;
};

#define CTRL(x) Qt::CTRL+Qt::Key_##x
#define SHIFT(x) Qt::SHIFT+Qt::Key_##x
#define CTRLSHIFT(x) Qt::CTRL+Qt::SHIFT+Qt::Key_##x
#define ALT(x) Qt::ALT+Qt::Key_##x
#define ALTSHIFT(x) Qt::ALT+Qt::SHIFT+Qt::Key_##x
#define NOSTD QKeySequence::UnknownKey

static KStandardShortcutInfo g_infoStandardShortcut[] =
{
    {AccelNone, 0      , 0                   , 0       , 0      , 0           , NOSTD, KShortcut(), false },
    { Open    , "Open" , "@action", "Open" , CTRL(O), 0           , QKeySequence::Open, KShortcut(), false },
    { New     , "New"  , "@action", "New"  , CTRL(N), 0           , QKeySequence::New, KShortcut(), false },
    { Close   , "Close", "@action", "Close", CTRL(W), CTRL(Escape), QKeySequence::Close, KShortcut(), false },
    { Save    , "Save" , "@action", "Save" , CTRL(S), 0           , QKeySequence::Save, KShortcut(), false },
    { Print   , "Print", "@action", "Print", CTRL(P), 0           , QKeySequence::Print, KShortcut(), false },
    { Quit    , "Quit" , "@action", "Quit" , CTRL(Q), 0           , NOSTD, KShortcut(), false },
    { Undo             , "Undo"             , "@action", "Undo"                 , CTRL(Z)          , 0            , QKeySequence::Undo, KShortcut(), false },
    { Redo             , "Redo"             , "@action", "Redo"                 , CTRLSHIFT(Z)     , 0            , QKeySequence::Redo, KShortcut(), false },
    { Cut              , "Cut"              , "@action", "Cut"                  , CTRL(X)          , SHIFT(Delete), QKeySequence::Cut, KShortcut(), false },
    { Copy             , "Copy"             , "@action", "Copy"                 , CTRL(C)          , CTRL(Insert) , QKeySequence::Copy, KShortcut(), false },
    { Paste            , "Paste"            , "@action", "Paste"                , CTRL(V)          , SHIFT(Insert), QKeySequence::Paste, KShortcut(), false },
    { PasteSelection   , "Paste Selection"  , "@action", "Paste Selection"      , CTRLSHIFT(Insert), 0            , NOSTD, KShortcut(), false },
    { SelectAll        , "SelectAll"        , "@action", "Select All"           , CTRL(A)          , 0            , QKeySequence::SelectAll, KShortcut(), false },
    { Deselect         , "Deselect"         , "@action", "Deselect"             , CTRLSHIFT(A)     , 0            , NOSTD, KShortcut(), false },
    { DeleteWordBack   , "DeleteWordBack"   , "@action", "Delete Word Backwards", CTRL(Backspace)  , 0            , QKeySequence::DeleteStartOfWord, KShortcut(), false },
    { DeleteWordForward, "DeleteWordForward", "@action", "Delete Word Forward"  , CTRL(Delete)     , 0            , QKeySequence::DeleteEndOfWord, KShortcut(), false },
    { Find             , "Find"             , "@action", "Find"                 , CTRL(F)          , 0            , QKeySequence::Find, KShortcut(), false },
    { FindNext         , "FindNext"         , "@action", "Find Next"            , Qt::Key_F3       , 0            , QKeySequence::FindNext, KShortcut(), false },
    { FindPrev         , "FindPrev"         , "@action", "Find Prev"            , SHIFT(F3)        , 0            , QKeySequence::FindPrevious, KShortcut(), false },
    { Replace          , "Replace"          , "@action", "Replace"              , CTRL(R)          , 0            , QKeySequence::Replace, KShortcut(), false },
    { Home           , "Home"                 , "@action Go to main page"      , "Home"                 , ALT(Home)       , Qt::Key_HomePage  , NOSTD, KShortcut(), false },
    { Begin          , "Begin"                , "@action Beginning of document", "Begin"                , CTRL(Home)      , 0                 , QKeySequence::MoveToStartOfDocument, KShortcut(), false },
    { End            , "End"                  , "@action End of document"      , "End"                  , CTRL(End)       , 0                 , QKeySequence::MoveToEndOfDocument, KShortcut(), false },
    { Prior          , "Prior"                , "@action"                      , "Prior"                , Qt::Key_PageUp  , 0                 , QKeySequence::MoveToPreviousPage, KShortcut(), false },
    { Next           , "Next"                 , "@action Opposite to Prior"    , "Next"                 , Qt::Key_PageDown, 0                 , QKeySequence::MoveToNextPage, KShortcut(), false },
    { Up             , "Up"                   , "@action"                      , "Up"                   , ALT(Up)         , 0                 , NOSTD, KShortcut(), false },
    { Back           , "Back"                 , "@action"                      , "Back"                 , ALT(Left)       , Qt::Key_Back      , QKeySequence::Back, KShortcut(), false },
    { Forward        , "Forward"              , "@action"                      , "Forward"              , ALT(Right)      , Qt::Key_Forward   , QKeySequence::Forward, KShortcut(), false },
    { Reload         , "Reload"               , "@action"                      , "Reload"               , Qt::Key_F5      , Qt::Key_Refresh   , QKeySequence::Refresh, KShortcut(), false },
    { BeginningOfLine, "BeginningOfLine"      , "@action"                      , "Beginning of Line"    , Qt::Key_Home    , 0                 , QKeySequence::MoveToStartOfLine, KShortcut(), false },
    { EndOfLine      , "EndOfLine"            , "@action"                      , "End of Line"          , Qt::Key_End     , 0                 , QKeySequence::MoveToEndOfLine, KShortcut(), false },
    { GotoLine       , "GotoLine"             , "@action"                      , "Go to Line"           , CTRL(G)         , 0                 , NOSTD, KShortcut(), false },
    { BackwardWord   , "BackwardWord"         , "@action"                      , "Backward Word"        , CTRL(Left)      , 0                 , QKeySequence::MoveToPreviousWord, KShortcut(), false },
    { ForwardWord    , "ForwardWord"          , "@action"                      , "Forward Word"         , CTRL(Right)     , 0                 , QKeySequence::MoveToNextWord, KShortcut(), false },
    { AddBookmark    , "AddBookmark"          , "@action"                      , "Add Bookmark"         , CTRL(B)         , 0                 , NOSTD, KShortcut(), false },
    { ZoomIn         , "ZoomIn"               , "@action"                      , "Zoom In"              , CTRL(Plus)      , CTRL(Equal)       , QKeySequence::ZoomIn, KShortcut(), false },
    { ZoomOut        , "ZoomOut"              , "@action"                      , "Zoom Out"             , CTRL(Minus)     , 0                 , QKeySequence::ZoomOut, KShortcut(), false },
    { FullScreen     , "FullScreen"           , "@action"                      , "Full Screen Mode"     , CTRLSHIFT(F)    , 0                 , NOSTD, KShortcut(), false },
    { ShowMenubar    , "ShowMenubar"          , "@action"                      , "Show Menu Bar"        , CTRL(M)         , 0                 , NOSTD, KShortcut(), false },
    { TabNext        , "Activate Next Tab"    , "@action"                      , "Activate Next Tab"    , CTRL(Period)    , CTRL(BracketRight), NOSTD, KShortcut(), false },
    { TabPrev        , "Activate Previous Tab", "@action"                      , "Activate Previous Tab", CTRL(Comma)     , CTRL(BracketLeft) , NOSTD, KShortcut(), false },
    { Help           , "Help"                 , "@action"                      , "Help"                 , Qt::Key_F1      , 0                 , QKeySequence::HelpContents, KShortcut(), false },
    { WhatsThis      , "WhatsThis"            , "@action"                      , "What's This"          , SHIFT(F1)       , 0                 , QKeySequence::WhatsThis, KShortcut(), false },
    { TextCompletion           , "TextCompletion"           , "@action", "Text Completion"          , CTRL(E)     , 0, NOSTD, KShortcut(), false },
    { PrevCompletion           , "PrevCompletion"           , "@action", "Previous Completion Match", CTRL(Up)    , 0, NOSTD, KShortcut(), false },
    { NextCompletion           , "NextCompletion"           , "@action", "Next Completion Match"    , CTRL(Down)  , 0, NOSTD, KShortcut(), false },
    { SubstringCompletion      , "SubstringCompletion"      , "@action", "Substring Completion"     , CTRL(T)     , 0, NOSTD, KShortcut(), false },
    { RotateUp                 , "RotateUp"                 , "@action", "Previous Item in List"    , Qt::Key_Up  , 0, NOSTD, KShortcut(), false },
    { RotateDown               , "RotateDown"               , "@action", "Next Item in List"        , Qt::Key_Down, 0, NOSTD, KShortcut(), false },
    { OpenRecent               , "OpenRecent"               , "@action", "Open Recent"              , 0           , 0, NOSTD, KShortcut(), false },
    { SaveAs                   , "SaveAs"                   , "@action", "Save As"                  , CTRLSHIFT(S), 0, QKeySequence::SaveAs, KShortcut(), false },
    { Revert                   , "Revert"                   , "@action", "Revert"                   , 0           , 0, NOSTD, KShortcut(), false },
    { PrintPreview             , "PrintPreview"             , "@action", "Print Preview"            , 0           , 0, NOSTD, KShortcut(), false },
    { Mail                     , "Mail"                     , "@action", "Mail"                     , 0           , 0, NOSTD, KShortcut(), false },
    { Clear                    , "Clear"                    , "@action", "Clear"                    , 0           , 0, NOSTD, KShortcut(), false },
    { ActualSize               , "ActualSize"               , "@action", "Actual Size"              , 0           , 0, NOSTD, KShortcut(), false },
    { FitToPage                , "FitToPage"                , "@action", "Fit To Page"              , 0           , 0, NOSTD, KShortcut(), false },
    { FitToWidth               , "FitToWidth"               , "@action", "Fit To Width"             , 0           , 0, NOSTD, KShortcut(), false },
    { FitToHeight              , "FitToHeight"              , "@action", "Fit To Height"            , 0           , 0, NOSTD, KShortcut(), false },
    { Zoom                     , "Zoom"                     , "@action", "Zoom"                     , 0           , 0, NOSTD, KShortcut(), false },
    { Goto                     , "Goto"                     , "@action", "Goto"                     , 0           , 0, NOSTD, KShortcut(), false },
    { GotoPage                 , "GotoPage"                 , "@action", "Goto Page"                , 0           , 0, NOSTD, KShortcut(), false },
    { DocumentBack             , "DocumentBack"             , "@action", "Document Back"            , ALTSHIFT(Left), 0, NOSTD, KShortcut(), false },
    { DocumentForward          , "DocumentForward"          , "@action", "Document Forward"         , ALTSHIFT(Right), 0, NOSTD, KShortcut(), false },
    { EditBookmarks            , "EditBookmarks"            , "@action", "Edit Bookmarks"           , 0           , 0, NOSTD, KShortcut(), false },
    { Spelling                 , "Spelling"                 , "@action", "Spelling"                 , 0           , 0, NOSTD, KShortcut(), false },
    { ShowToolbar              , "ShowToolbar"              , "@action", "Show Toolbar"             , 0           , 0, NOSTD, KShortcut(), false },
    { ShowStatusbar            , "ShowStatusbar"            , "@action", "Show Statusbar"           , 0           , 0, NOSTD, KShortcut(), false },
    { SaveOptions              , "SaveOptions"              , "@action", "Save Options"             , 0           , 0, NOSTD, KShortcut(), false },
    { KeyBindings              , "KeyBindings"              , "@action", "Key Bindings"             , 0           , 0, NOSTD, KShortcut(), false },
    { Preferences              , "Preferences"              , "@action", "Preferences"              , 0           , 0, QKeySequence::Preferences, KShortcut(), false },
    { ConfigureToolbars        , "ConfigureToolbars"        , "@action", "Configure Toolbars"       , 0           , 0, NOSTD, KShortcut(), false },
    { ConfigureNotifications   , "ConfigureNotifications"   , "@action", "Configure Notifications"  , 0           , 0, NOSTD, KShortcut(), false },
    { TipofDay                 , "TipofDay"                 , "@action", "Tip Of Day"               , 0           , 0, NOSTD, KShortcut(), false },
    { ReportBug                , "ReportBug"                , "@action", "Report Bug"               , 0           , 0, NOSTD, KShortcut(), false },
    { SwitchApplicationLanguage, "SwitchApplicationLanguage", "@action", "Switch Application Language", 0         , 0, NOSTD, KShortcut(), false },
    { AboutApp                 , "AboutApp"                 , "@action", "About Application"        , 0           , 0, NOSTD, KShortcut(), false },
    { AboutKDE                 , "AboutKDE"                 , "@action", "About KDE"                , 0           , 0, NOSTD, KShortcut(), false },
    { StandardShortcutCount    , 0                          , 0        , 0                          , 0           , 0, NOSTD, KShortcut(), false }
};

static KStandardShortcutInfo *guardedStandardShortcutInfo(StandardShortcut id)
{
    if (id >= static_cast<int>(sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo)) || id < 0) {
        kWarning(125) << "KStandardShortcut: id not found!";
        return &g_infoStandardShortcut[AccelNone];
    }
    return &g_infoStandardShortcut[id];
}

static void initialize(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);

    KConfigGroup cg(KGlobal::config(), "Shortcuts");
    if (info->name && cg.hasKey(info->name)) {
        QString s = cg.readEntry(info->name);
        if (s != QLatin1String("none"))
            info->cut = KShortcut(s);
        else
            info->cut = KShortcut();
    } else {
        info->cut = hardcodedDefaultShortcut(id);
    }
    info->isInitialized = true;
}

void saveShortcut(StandardShortcut id, const KShortcut &newShortcut)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);
    if (info->id == AccelNone)
        return;

    KConfigGroup cg(KGlobal::config(), "Shortcuts");
    info->cut = newShortcut;
    const bool sameAsDefault = (newShortcut == hardcodedDefaultShortcut(id));
    if (sameAsDefault) {
        if (cg.hasKey(info->name))
            cg.deleteEntry(info->name, KConfig::Global | KConfig::Persistent);
        return;
    }
    cg.writeEntry(info->name, info->cut.toString(), KConfig::Global | KConfig::Persistent);
}

QString name(StandardShortcut id)
{
    return QString::fromLatin1(guardedStandardShortcutInfo(id)->name);
}

QString label(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);
    if (!info->description)
        return QString();
    return i18nc(info->translation_context, info->description);
}

QString whatsThis(StandardShortcut)
{
    return QString();
}

const KShortcut &shortcut(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);
    if (!info->isInitialized)
        initialize(id);
    return info->cut;
}

StandardShortcut find(const QKeySequence &seq)
{
    if (!seq.isEmpty()) {
        for (uint i = 0; i < sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo); i++) {
            StandardShortcut id = g_infoStandardShortcut[i].id;
            if (id != AccelNone && id != StandardShortcutCount) {
                if (!g_infoStandardShortcut[i].isInitialized)
                    initialize(id);
                if (g_infoStandardShortcut[i].cut.contains(seq))
                    return id;
            }
        }
    }
    return AccelNone;
}

StandardShortcut find(const char *keyName)
{
    for (uint i = 0; i < sizeof(g_infoStandardShortcut) / sizeof(KStandardShortcutInfo); i++)
        if (g_infoStandardShortcut[i].name && qstrcmp(g_infoStandardShortcut[i].name, keyName) == 0)
            return g_infoStandardShortcut[i].id;
    return AccelNone;
}

KShortcut hardcodedDefaultShortcut(StandardShortcut id)
{
    KStandardShortcutInfo *info = guardedStandardShortcutInfo(id);
    if (info->stdKey != QKeySequence::UnknownKey) {
        const QList<QKeySequence> bindings = QKeySequence::keyBindings(info->stdKey);
        if (!bindings.isEmpty()) {
            KShortcut cut(bindings.at(0));
            if (bindings.count() > 1)
                cut.setAlternate(bindings.at(1));
            return cut;
        }
    }
    return KShortcut(info->cutDefault, info->cutDefault2);
}

const KShortcut &open()                  { return shortcut(Open); }
const KShortcut &openNew()               { return shortcut(New); }
const KShortcut &close()                 { return shortcut(Close); }
const KShortcut &save()                  { return shortcut(Save); }
const KShortcut &print()                 { return shortcut(Print); }
const KShortcut &quit()                  { return shortcut(Quit); }
const KShortcut &cut()                   { return shortcut(Cut); }
const KShortcut &copy()                  { return shortcut(Copy); }
const KShortcut &paste()                 { return shortcut(Paste); }
const KShortcut &pasteSelection()        { return shortcut(PasteSelection); }
const KShortcut &deleteWordBack()        { return shortcut(DeleteWordBack); }
const KShortcut &deleteWordForward()     { return shortcut(DeleteWordForward); }
const KShortcut &undo()                  { return shortcut(Undo); }
const KShortcut &redo()                  { return shortcut(Redo); }
const KShortcut &find()                  { return shortcut(Find); }
const KShortcut &findNext()              { return shortcut(FindNext); }
const KShortcut &findPrev()              { return shortcut(FindPrev); }
const KShortcut &replace()               { return shortcut(Replace); }
const KShortcut &home()                  { return shortcut(Home); }
const KShortcut &begin()                 { return shortcut(Begin); }
const KShortcut &end()                   { return shortcut(End); }
const KShortcut &beginningOfLine()       { return shortcut(BeginningOfLine); }
const KShortcut &endOfLine()             { return shortcut(EndOfLine); }
const KShortcut &prior()                 { return shortcut(Prior); }
const KShortcut &next()                  { return shortcut(Next); }
const KShortcut &backwardWord()          { return shortcut(BackwardWord); }
const KShortcut &forwardWord()           { return shortcut(ForwardWord); }
const KShortcut &gotoLine()              { return shortcut(GotoLine); }
const KShortcut &addBookmark()           { return shortcut(AddBookmark); }
const KShortcut &tabNext()               { return shortcut(TabNext); }
const KShortcut &tabPrev()               { return shortcut(TabPrev); }
const KShortcut &fullScreen()            { return shortcut(FullScreen); }
const KShortcut &zoomIn()                { return shortcut(ZoomIn); }
const KShortcut &zoomOut()               { return shortcut(ZoomOut); }
const KShortcut &help()                  { return shortcut(Help); }
const KShortcut &completion()            { return shortcut(TextCompletion); }
const KShortcut &prevCompletion()        { return shortcut(PrevCompletion); }
const KShortcut &nextCompletion()        { return shortcut(NextCompletion); }
const KShortcut &rotateUp()              { return shortcut(RotateUp); }
const KShortcut &rotateDown()            { return shortcut(RotateDown); }
const KShortcut &substringCompletion()   { return shortcut(SubstringCompletion); }
const KShortcut &whatsThis()             { return shortcut(WhatsThis); }
const KShortcut &reload()                { return shortcut(Reload); }
const KShortcut &selectAll()             { return shortcut(SelectAll); }
const KShortcut &up()                    { return shortcut(Up); }
const KShortcut &back()                  { return shortcut(Back); }
const KShortcut &forward()               { return shortcut(Forward); }
const KShortcut &showMenubar()           { return shortcut(ShowMenubar); }
const KShortcut &insert()                { static KShortcut s(Qt::Key_Insert); return s; }

}
