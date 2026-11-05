# Symbol Viewer plugin ("Symbol List" tool view): parses the current
# document with a small per-language parser (cpp_parser.cpp etc.) and lists
# functions/macros/structures in a tree. Upstream this is a dynamically
# loaded Kate/Plugin; here it is a static library linked into the kate
# executable, imported there by kate/app/katebuiltinplugins.cpp.

set(katesymbolviewerplugin_PART_SRCS
    cpp_parser.cpp
    tcl_parser.cpp
    fortran_parser.cpp
    perl_parser.cpp
    php_parser.cpp
    xslt_parser.cpp
    ruby_parser.cpp
    python_parser.cpp
    bash_parser.cpp
    ecma_parser.cpp
    plugin_katesymbolviewer.cpp
)

kate_add_static_plugin(katesymbolviewerplugin ${katesymbolviewerplugin_PART_SRCS})

target_link_libraries(katesymbolviewerplugin
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(kate/plugins/katesymbolviewer FILES ui.rc APPS kate)
kate_install_data(services FILES katesymbolviewer.desktop APPS kate)
