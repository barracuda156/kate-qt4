# CTags plugin ("CTags" tool view): generates/looks up a ctags tag database
# for the current session and a global one, jumps to definitions/
# declarations. Upstream this is a dynamically loaded Kate/Plugin; here it
# is a static library linked into the kate executable, imported there by
# kate/app/katebuiltinplugins.cpp. readtags.c/readtags.h are the vendored
# (public domain) Exuberant CTags tag-file reader, plain C.

set(katectagsplugin_PART_SRCS
    readtags.c
    tags.cpp
    ctagskinds.cpp
    kate_ctags_view.cpp
    kate_ctags_plugin.cpp
)

kate_add_ui_files(katectagsplugin_PART_SRCS kate_ctags.ui CTagsGlobalConfig.ui)

kate_add_static_plugin(katectagsplugin ${katectagsplugin_PART_SRCS})

target_link_libraries(katectagsplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katectags FILES ui.rc APPS kate)
kate_install_data(services FILES katectagsplugin.desktop APPS kate)
