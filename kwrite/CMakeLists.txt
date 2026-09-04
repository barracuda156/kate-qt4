# KWrite: simple SDI editor shell on top of KatePart (pure Qt 4)

set(kwrite_SRCS kwritemain.cpp kwriteapp.cpp)
kate_automoc(kwrite_SRCS ${kwrite_SRCS})

if(APPLE)
    # Application icon: a prebuilt .icns is used when present, otherwise it is
    # generated at build time with sips (available on every macOS since 10.4)
    # from the 128px Oxygen "accessories-text-editor" PNG.
    set(kwrite_ICON_SRCS)
    set(kwrite_ICON_PNG ${CMAKE_SOURCE_DIR}/qt4compat/data/icons/oxygen/128x128/apps/accessories-text-editor.png)
    if(EXISTS ${CMAKE_SOURCE_DIR}/qt4compat/data/kwrite.icns)
        set(kwrite_ICON_SRCS ${CMAKE_SOURCE_DIR}/qt4compat/data/kwrite.icns)
    elseif(EXISTS ${kwrite_ICON_PNG})
        find_program(SIPS_EXECUTABLE sips)
        if(SIPS_EXECUTABLE)
            set(kwrite_ICON_SRCS ${CMAKE_CURRENT_BINARY_DIR}/kwrite.icns)
            add_custom_command(OUTPUT ${kwrite_ICON_SRCS}
                COMMAND ${SIPS_EXECUTABLE} -s format icns ${kwrite_ICON_PNG} --out ${kwrite_ICON_SRCS}
                DEPENDS ${kwrite_ICON_PNG}
                COMMENT "Generating kwrite.icns with sips"
                VERBATIM)
            set_source_files_properties(${kwrite_ICON_SRCS} PROPERTIES GENERATED TRUE)
        endif()
    endif()
    if(kwrite_ICON_SRCS)
        set_source_files_properties(${kwrite_ICON_SRCS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set(MACOSX_BUNDLE_ICON_FILE kwrite.icns)
    endif()
    add_executable(kwrite MACOSX_BUNDLE ${kwrite_SRCS} ${kwrite_ICON_SRCS})
    set_target_properties(kwrite PROPERTIES
        OUTPUT_NAME KWrite
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in
        MACOSX_BUNDLE_BUNDLE_NAME "KWrite"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.kwrite"
        MACOSX_BUNDLE_INFO_STRING "KWrite - Text Editor"
        MACOSX_BUNDLE_LONG_VERSION_STRING "4.14.3"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "4.14.3"
        MACOSX_BUNDLE_BUNDLE_VERSION "4.14.3"
        MACOSX_BUNDLE_COPYRIGHT "(c) 2000-2013 The Kate Authors"
    )
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

kate_install_data(kwrite FILES kwriteui.rc)
