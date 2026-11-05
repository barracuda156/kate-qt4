# Snippets plugin: embeds the snippet widget KatePart's editor component
# exposes through the "snippetWidget" property (set only when KatePart was
# built with snippet support) in a Kate tool view with its own KToolBar. No
# ui.rc: the tool view has no XMLGUI actions of its own, only the snippet
# widget's. Upstream this is a dynamically loaded Kate/Plugin; here it is a
# static library linked into the kate executable, imported there by
# kate/app/katebuiltinplugins.cpp.

set(katesnippetsplugin_PART_SRCS
    katesnippets.cpp
)

kate_add_static_plugin(katesnippetsplugin ${katesnippetsplugin_PART_SRCS})

target_link_libraries(katesnippetsplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(services FILES katesnippetsplugin.desktop APPS kate)
