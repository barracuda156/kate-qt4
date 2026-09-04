# Document tree ("File Tree") plugin. Upstream this is a dynamically loaded
# Kate/Plugin that Kate always loads (X-Kate-LoadAlways=True); here it is a
# static library linked into the kate executable, imported there by
# kate/app/katebuiltinplugins.cpp. The service file and the XMLGUI
# definition are installed as data so KServiceTypeTrader finds the plugin
# and Kate::XMLGUIClient finds kate/plugins/filetree/ui.rc.

set(katefiletreeplugin_PART_SRCS
    katefiletreeplugin.cpp
    katefiletree.cpp
    katefiletreemodel.cpp
    katefiletreeproxymodel.cpp
    katefiletreeconfigpage.cpp
    katefiletreepluginsettings.cpp
)

kate_add_static_plugin(katefiletreeplugin ${katefiletreeplugin_PART_SRCS})

target_link_libraries(katefiletreeplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/filetree FILES ui.rc APPS kate)
kate_install_data(services FILES katefiletreeplugin.desktop APPS kate)
