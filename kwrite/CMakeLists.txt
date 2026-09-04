# KWrite: simple SDI editor shell on top of KatePart (pure Qt 4)

set(kwrite_SRCS kwritemain.cpp kwriteapp.cpp)
kate_automoc(kwrite_SRCS ${kwrite_SRCS})

if(APPLE)
    kate_macos_app_icon(kwrite_ICON_SRCS kwrite.icns
        ${CMAKE_SOURCE_DIR}/qt4compat/data/icons/oxygen/128x128/apps/accessories-text-editor.png)
    add_executable(kwrite MACOSX_BUNDLE ${kwrite_SRCS} ${kwrite_ICON_SRCS})
    kate_set_bundle_properties(kwrite KWrite org.kde.kwrite "KWrite - Text Editor")
else()
    add_executable(kwrite ${kwrite_SRCS})
endif()

target_link_libraries(kwrite
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
kate_link_whole_archive(kwrite katepartinterfaces)
if(BZIP2_FOUND)
    target_link_libraries(kwrite ${BZIP2_LIBRARIES})
endif()

add_dependencies(kwrite kate-data)

if(APPLE)
    install(TARGETS kwrite BUNDLE DESTINATION ${KATE_INSTALL_BUNDLE_DIR})
else()
    install(TARGETS kwrite RUNTIME DESTINATION ${KATE_BIN_INSTALL_DIR})
    install(PROGRAMS kwrite.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()

kate_install_data(kwrite FILES kwriteui.rc APPS kwrite)
