# Search & Replace plugin ("Search and Replace" tool view): search in a
# folder, in open files or in the current project; plain-text or regular-
# expression matching on KatePart's pcre2-based engine (KateRegExp/KatePcre,
# part/search and part/utils); replace with per-match checkboxes; folder
# listing and disk-file search run on background QThreads. Upstream this is
# a dynamically loaded Kate/Plugin; here it is a static library linked into
# the kate executable, imported there by kate/app/katebuiltinplugins.cpp.
# The service file and the XMLGUI definition are installed as data so
# KServiceTypeTrader finds the plugin and Kate::XMLGUIClient finds
# kate/plugins/katesearch/ui.rc.

set(katesearchplugin_PART_SRCS
    plugin_search.cpp
    search_open_files.cpp
    SearchDiskFiles.cpp
    FolderFilesList.cpp
    replace_matches.cpp
    htmldelegate.cpp
)

kate_add_ui_files(katesearchplugin_PART_SRCS search.ui results.ui)

# kateregexp.h / katepcre.h (part/search, part/utils) - the pcre2-based
# regex engine this plugin's disk/open-file search and replace use instead
# of QRegExp.
include_directories(${CMAKE_SOURCE_DIR}/part/search ${CMAKE_SOURCE_DIR}/part/utils)

kate_add_static_plugin(katesearchplugin ${katesearchplugin_PART_SRCS})

target_link_libraries(katesearchplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katesearch FILES ui.rc APPS kate)
kate_install_data(services FILES katesearch.desktop APPS kate)
