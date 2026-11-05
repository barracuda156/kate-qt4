# Text Filter plugin: pipes the selection, or the whole document, through a
# user-given shell command (KProcess) and replaces it with the command's
# output, or copies the output to the clipboard; also registers the
# "textfilter" editor command. Upstream this is a dynamically loaded
# Kate/Plugin; here it is a static library linked into the kate executable,
# imported there by kate/app/katebuiltinplugins.cpp.

set(katetextfilterplugin_PART_SRCS
    plugin_katetextfilter.cpp
)

kate_add_ui_files(katetextfilterplugin_PART_SRCS textfilterwidget.ui)

kate_add_static_plugin(katetextfilterplugin ${katetextfilterplugin_PART_SRCS})

target_link_libraries(katetextfilterplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katetextfilter FILES ui.rc APPS kate)
kate_install_data(services FILES katetextfilter.desktop APPS kate)
