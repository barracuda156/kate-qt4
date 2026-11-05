# Kate application plugins built into the kate executable. Add a directory
# here once it is ported (kate_add_static_plugin + kate_install_data for its
# .desktop under services/ and its ui.rc under kate/plugins/<component>/).
# Not built: konsole, mailfiles (not needed for a text editor), katesql,
# replicode, pate, kttsd (see ROADMAP-QT4.md).

include_directories(
    ${CMAKE_SOURCE_DIR}/kate/interfaces
    ${CMAKE_SOURCE_DIR}/kate/app
)

add_subdirectory(search)
add_subdirectory(symbolviewer)
add_subdirectory(kate-ctags)
add_subdirectory(tabbarextension)
add_subdirectory(textfilter)
add_subdirectory(openheader)
add_subdirectory(backtracebrowser)
add_subdirectory(xmlcheck)
add_subdirectory(snippets)
