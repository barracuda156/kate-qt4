# XML Checker plugin: runs xmllint on the current document (via a temp
# file) and lists well-formedness/validation errors, double-clickable to
# jump to the line. Upstream this is a dynamically loaded Kate/Plugin; here
# it is a static library linked into the kate executable, imported there by
# kate/app/katebuiltinplugins.cpp.

set(katexmlcheckplugin_PART_SRCS plugin_katexmlcheck.cpp)

kate_add_static_plugin(katexmlcheckplugin ${katexmlcheckplugin_PART_SRCS})

target_link_libraries(katexmlcheckplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katexmlcheck FILES ui.rc APPS kate)
kate_install_data(services FILES katexmlcheck.desktop APPS kate)
