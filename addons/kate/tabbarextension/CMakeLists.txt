# Tab Bar Extension plugin: an extra tab bar with one button per open
# document (KTinyTabBar), configurable location/size/sorting and a dialog
# for highlighting modified/active/previous tabs (KTinyTabBarConfigDialog,
# KTinyTabBarConfigPage). Upstream this is a dynamically loaded Kate/Plugin;
# here it is a static library linked into the kate executable, imported
# there by kate/app/katebuiltinplugins.cpp.

set(katetabbarextensionplugin_PART_SRCS
    plugin_katetabbarextension.cpp
    ktinytabbutton.cpp
    ktinytabbar.cpp
    ktinytabbarconfigdialog.cpp
    ktinytabbarconfigpage.cpp
)

kate_add_ui_files(katetabbarextensionplugin_PART_SRCS tabbarconfigwidget.ui)

kate_add_static_plugin(katetabbarextensionplugin ${katetabbarextensionplugin_PART_SRCS})

target_link_libraries(katetabbarextensionplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katetabbarextension FILES ui.rc APPS kate)
kate_install_data(services FILES katetabbarextension.desktop APPS kate)
