#!/bin/sh
# qt4compat/tools/gen-forwarders.sh - create the CamelCase forwarding headers
# under include/KDE (include/KDE/KConfig -> "../kconfig.h",
# include/KDE/KParts/Part -> "../../kparts/part.h") for every mapping listed
# below. They live in their own directory, as kdelibs installs them, because
# KIO/ next to kio/ (or KParts/ next to kparts/) collides on case-insensitive
# file systems. Regenerate the list from disk with:
#   for f in $(cd include/KDE && find . -type f | sort); do ...
HERE="$(cd "$(dirname "$0")" && pwd)"
INC="$HERE/../include/KDE"
gen() {
    # $1 = CamelCase name (may contain one directory), $2 = target header
    # relative to include/
    case "$1" in
        */*) mkdir -p "$INC/${1%/*}"; printf '#include "../../%s"\n' "$2" > "$INC/$1" ;;
        *)   printf '#include "../%s"\n' "$2" > "$INC/$1" ;;
    esac
}
while read cc hdr; do
    [ -z "$cc" ] && continue
    case "$cc" in \#*) continue ;; esac
    gen "$cc" "$hdr"
done <<'LIST'
KAboutApplicationDialog kaboutapplicationdialog.h
KAboutData kaboutdata.h
KAboutPerson kaboutdata.h
KAcceleratorManager kacceleratormanager.h
KAction kaction.h
KActionCollection kactioncollection.h
KActionMenu kactionmenu.h
KApplication kapplication.h
KAuthorized kauthorized.h
KCategorizedSortFilterProxyModel kcategorizedsortfilterproxymodel.h
KCategorizedView kcategorizedview.h
KCategoryDrawer kcategorydrawer.h
KCategoryDrawerV3 kcategorydrawer.h
KCharsets kcharsets.h
KCmdLineArgs kcmdlineargs.h
KCmdLineOptions kcmdlineargs.h
KCodecs kcodecs.h
KColorButton kcolorbutton.h
KColorCombo kcolorcombo.h
KColorDialog kcolordialog.h
KColorScheme kcolorscheme.h
KColorUtils kcolorutils.h
KComboBox kcombobox.h
KCompletion kcompletion.h
KCompletionBox kcompletionbox.h
KComponentData kcomponentdata.h
KConfig kconfig.h
KConfigBase kconfigbase.h
KConfigGroup kconfiggroup.h
KCursor kcursor.h
KDebug kdebug.h
KDialog kdialog.h
KDirWatch kdirwatch.h
KDoubleNumInput knuminput.h
KEditToolBar kedittoolbar.h
KEncodingFileDialog kencodingfiledialog.h
KEncodingProber kencodingprober.h
KFileDialog kfiledialog.h
KFileItem kfileitem.h
KFilterDev kfilterdev.h
KFontChooser kfontchooser.h
KFontComboBox kfontcombobox.h
KFontDialog kfontdialog.h
KGlobal kglobal.h
KGlobalSettings kglobalsettings.h
KGuiItem kguiitem.h
KHBox khbox.h
KHistoryComboBox khistorycombobox.h
KIO/CopyJob kio/copyjob.h
KIO/DeleteJob kio/deletejob.h
KIO/Global kio/global.h
KIO/Job kio/job.h
KIO/ListJob kio/jobclasses.h
KIO/NetAccess kio/netaccess.h
KIcon kicon.h
KIconButton kicondialog.h
KIconDialog kicondialog.h
KIconLoader kiconloader.h
KIconTheme kicontheme.h
KInputDialog kinputdialog.h
KIntNumInput knuminput.h
KIntSpinBox knuminput.h
KLibLoader klibloader.h
KLineEdit klineedit.h
KListWidget klistwidget.h
KLocale klocale.h
KLocalizedString klocalizedstring.h
KMD5 kcodecs.h
KMainWindow kmainwindow.h
KMenu kmenu.h
KMenuBar kmenubar.h
KMessageBox kmessagebox.h
KMessageWidget kmessagewidget.h
KMimeType kmimetype.h
KMimeTypeChooser kmimetypechooser.h
KMimeTypeChooserDialog kmimetypechooser.h
KMimeTypeTrader kmimetypetrader.h
KMultiTabBar kmultitabbar.h
KNotification knotification.h
KNumInput knuminput.h
KOpenWithDialog kopenwithdialog.h
KPageDialog kpagedialog.h
KPageWidget kpagewidget.h
KPageWidgetItem kpagewidgetmodel.h
KParts/BrowserExtension kparts/browserextension.h
KParts/Event kparts/event.h
KParts/Factory kparts/factory.h
KParts/MainWindow kparts/mainwindow.h
KParts/Part kparts/part.h
KParts/PartManager kparts/partmanager.h
KParts/Plugin kparts/plugin.h
KPluginFactory kpluginfactory.h
KPluginInfo kplugininfo.h
KPluginLoader kpluginloader.h
KProcess kprocess.h
KProgressDialog kprogressdialog.h
KPushButton kpushbutton.h
KRecentFilesAction krecentfilesaction.h
KRun krun.h
KSaveFile ksavefile.h
KSelectAction kselectaction.h
KSeparator kseparator.h
KService kservice.h
KServiceTypeTrader kservicetypetrader.h
KSharedConfig ksharedconfig.h
KSharedPtr ksharedptr.h
KShell kshell.h
KShortcut kshortcut.h
KShortcutWidget kshortcutwidget.h
KShortcutsDialog kshortcutsdialog.h
KShortcutsEditor kshortcutseditor.h
KSqueezedTextLabel ksqueezedtextlabel.h
KStandardAction kstandardaction.h
KStandardDirs kstandarddirs.h
KStandardGuiItem kstandardguiitem.h
KStandardShortcut kstandardshortcut.h
KStartupInfo kstartupinfo.h
KStartupInfoId kstartupinfo.h
KStatusBar kstatusbar.h
KStringHandler kstringhandler.h
KTabBar ktabbar.h
KTabWidget ktabwidget.h
KTemporaryFile ktemporaryfile.h
KTextEdit ktextedit.h
KTipDatabase ktip.h
KTipDialog ktip.h
KTitleWidget ktitlewidget.h
KToggleAction ktoggleaction.h
KToggleFullScreenAction ktogglefullscreenaction.h
KToolBar ktoolbar.h
KToolInvocation ktoolinvocation.h
KUrl kurl.h
KUrlComboRequester kurlrequester.h
KUrlCompletion kurlcompletion.h
KUrlRequester kurlrequester.h
KUser kuser.h
KVBox kvbox.h
KWindowInfo kwindowsystem.h
KWindowSystem kwindowsystem.h
KXMLGUIBuilder kxmlguibuilder.h
KXMLGUIClient kxmlguiclient.h
KXMLGUIFactory kxmlguifactory.h
KXmlGuiWindow kxmlguiwindow.h
LIST
