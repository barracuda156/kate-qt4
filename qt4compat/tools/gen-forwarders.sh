#!/bin/sh
# qt4compat/tools/gen-forwarders.sh - create CamelCase forwarding headers
# (include/KConfig -> "kconfig.h") for every mapping listed below.
HERE="$(cd "$(dirname "$0")" && pwd)"
INC="$HERE/../include"
gen() {
    # $1 = CamelCase name, $2 = target header (relative to include/)
    printf '#include "%s"\n' "$2" > "$INC/$1"
}
while read cc hdr; do
    [ -z "$cc" ] && continue
    case "$cc" in \#*) continue ;; esac
    gen "$cc" "$hdr"
done <<'LIST'
KAboutApplicationDialog kaboutapplicationdialog.h
KAboutData kaboutdata.h
KAboutPerson kaboutdata.h
KAction kaction.h
KActionCollection kactioncollection.h
KActionMenu kactionmenu.h
KApplication kapplication.h
KAuthorized kauthorized.h
KCategoryDrawer kcategorydrawer.h
KCategoryDrawerV3 kcategorydrawer.h
KCategorizedSortFilterProxyModel kcategorizedsortfilterproxymodel.h
KCategorizedView kcategorizedview.h
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
KComponentData kcomponentdata.h
KConfig kconfig.h
KConfigBase kconfigbase.h
KConfigGroup kconfiggroup.h
KCursor kcursor.h
KDebug kdebug.h
KDialog kdialog.h
KDirWatch kdirwatch.h
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
KIcon kicon.h
KIconLoader kiconloader.h
KIconTheme kicontheme.h
KInputDialog kinputdialog.h
KIntNumInput knuminput.h
KIntSpinBox knuminput.h
KLibLoader klibloader.h
KLineEdit klineedit.h
KLocale klocale.h
KLocalizedString klocalizedstring.h
KMainWindow kmainwindow.h
KMD5 kcodecs.h
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
KOpenWithDialog kopenwithdialog.h
KPageDialog kpagedialog.h
KPageWidget kpagewidget.h
KPageWidgetItem kpagewidgetmodel.h
KPluginFactory kpluginfactory.h
KPluginInfo kplugininfo.h
KPluginLoader kpluginloader.h
KPluginSelector kpluginselector.h
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
KShortcutsDialog kshortcutsdialog.h
KShortcutsEditor kshortcutseditor.h
KSqueezedTextLabel ksqueezedtextlabel.h
KStandardAction kstandardaction.h
KStandardDirs kstandarddirs.h
KStandardGuiItem kstandardguiitem.h
KStandardShortcut kstandardshortcut.h
KStartupInfo kstartupinfo.h
KStatusBar kstatusbar.h
KStringHandler kstringhandler.h
KTabWidget ktabwidget.h
KTemporaryFile ktemporaryfile.h
KTextEdit ktextedit.h
KTipDialog ktip.h
KToggleAction ktoggleaction.h
KToggleFullScreenAction ktogglefullscreenaction.h
KToolBar ktoolbar.h
KToolInvocation ktoolinvocation.h
KUrl kurl.h
KUser kuser.h
KVBox kvbox.h
KWindowSystem kwindowsystem.h
KXMLGUIBuilder kxmlguibuilder.h
KXMLGUIClient kxmlguiclient.h
KXMLGUIFactory kxmlguifactory.h
KXmlGuiWindow kxmlguiwindow.h
LIST
echo "forwarders written to $INC"
