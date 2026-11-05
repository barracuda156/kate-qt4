# Kate application core.
#
# kateinterfaces: the Kate::* plugin interfaces (../interfaces/kate) and the
# application classes, one static library as everywhere in this build (the
# upstream shared library existed for out-of-tree plugins, which cannot be
# loaded here anyway).
#
# kate: the executable, katemain.cpp plus katebuiltinplugins.cpp, generated
# by kate_builtin_plugins_source() with one K_IMPORT_PLUGIN() per plugin that
# registered itself with kate_add_static_plugin() (Kate plugins: kate/filetree
# and addons/kate) or kate_add_static_editor_plugin() (KatePart editor
# plugins: addons/ktexteditor), directories the root CMakeLists processes
# before this one; that is what makes the statically linked plugins loadable
# through KService/KPluginLoader (see qt4compat/include/kpluginloader.h).

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(kateinterfaces_LIB_SRCS
   ../interfaces/kate/application.cpp
   ../interfaces/kate/mainwindow.cpp
   ../interfaces/kate/documentmanager.cpp
   ../interfaces/kate/pluginmanager.cpp
   ../interfaces/kate/plugin.cpp
   ../interfaces/kate/pluginconfigpageinterface.cpp
   kateapp.cpp
   kateconfigdialog.cpp
   kateconfigplugindialogpage.cpp
   katedocmanager.cpp
   katemainwindow.cpp
   katepluginmanager.cpp
   kateviewmanager.cpp
   kateviewspace.cpp
   katesavemodifieddialog.cpp
   katemwmodonhddialog.cpp
   katesession.cpp
   katemdi.cpp
   katecontainer.cpp
   kateappcommands.cpp
   katequickopen.cpp
)
kate_automoc(kateinterfaces_MOC_SRCS ${kateinterfaces_LIB_SRCS})

add_library(kateinterfaces STATIC ${kateinterfaces_LIB_SRCS} ${kateinterfaces_MOC_SRCS})
set_property(TARGET kateinterfaces APPEND PROPERTY COMPILE_DEFINITIONS MAKE_KATEINTERFACES_LIB)
target_link_libraries(kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTXML_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_builtin_plugins_source(kate_PLUGINS_SRC KATE_STATIC_PLUGINS KTEXTEDITOR_STATIC_PLUGINS)

set(kate_SRCS katemain.cpp ${kate_PLUGINS_SRC})
kate_automoc(kate_SRCS katemain.cpp)

if(APPLE)
    kate_macos_app_icon(kate_ICON_SRCS kate.icns
        ${CMAKE_SOURCE_DIR}/kate/data/icons/hi128-app-kate.png
        ${CMAKE_SOURCE_DIR}/kate/data/icons/hi32-app-kate.png
        ${CMAKE_SOURCE_DIR}/kate/data/icons/hi16-app-kate.png)
    add_executable(kate MACOSX_BUNDLE ${kate_SRCS} ${kate_ICON_SRCS})
    kate_set_bundle_properties(kate Kate org.kde.kate "Kate - Advanced Text Editor")
else()
    add_executable(kate ${kate_SRCS})
endif()

# the plugins come first: they depend on kateinterfaces (static link order)
target_link_libraries(kate
    ${kate_PLUGINS_SRC_LIBS}
    kateinterfaces
    ktexteditor
    qt4compat
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${PCRE2_LIBRARIES}
)
# katepartinterfaces must be force-linked whole so KatePart's self-registration
# with KTextEditor::EditorChooser actually runs; see kate_link_whole_archive().
kate_link_whole_archive(kate katepartinterfaces)
if(BZIP2_FOUND)
    target_link_libraries(kate ${BZIP2_LIBRARIES})
endif()

add_dependencies(kate kate-data)

if(APPLE)
    install(TARGETS kate BUNDLE DESTINATION ${KATE_INSTALL_BUNDLE_DIR})
else()
    install(TARGETS kate RUNTIME DESTINATION ${KATE_BIN_INSTALL_DIR})
endif()
