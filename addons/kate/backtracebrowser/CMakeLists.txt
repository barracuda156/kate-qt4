# Backtrace Browser plugin ("Backtrace Browser" tool view): parses a
# pasted/loaded gdb backtrace, jumps to the matching source line, using a
# background QThread (btfileindexer.cpp) to index configured search folders
# so relative filenames in the backtrace can be resolved. Upstream this is
# a dynamically loaded Kate/Plugin; here it is a static library linked into
# the kate executable, imported there by kate/app/katebuiltinplugins.cpp.
# No ui.rc: the plugin adds no menu/toolbar actions.

set(katebacktracebrowserplugin_PART_SRCS
    katebacktracebrowser.cpp
    btparser.cpp
    btfileindexer.cpp
    btdatabase.cpp
)

kate_add_ui_files(katebacktracebrowserplugin_PART_SRCS btbrowserwidget.ui btconfigwidget.ui)

kate_add_static_plugin(katebacktracebrowserplugin ${katebacktracebrowserplugin_PART_SRCS})

target_link_libraries(katebacktracebrowserplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(services FILES katebacktracebrowserplugin.desktop APPS kate)
