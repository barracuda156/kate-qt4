# Open Header plugin: F12, and the "toggle-header" editor command, switch
# between a C/C++ source file and its corresponding header by trying the
# common suffix pairs and case variants through KIO::NetAccess::exists
# (local files only here). Upstream this is a dynamically loaded
# Kate/Plugin; here it is a static library linked into the kate executable,
# imported there by kate/app/katebuiltinplugins.cpp.

set(kateopenheaderplugin_PART_SRCS
    plugin_kateopenheader.cpp
)

kate_add_static_plugin(kateopenheaderplugin ${kateopenheaderplugin_PART_SRCS})

target_link_libraries(kateopenheaderplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/kateopenheader FILES ui.rc APPS kate)
kate_install_data(services FILES kateopenheader.desktop APPS kate)
