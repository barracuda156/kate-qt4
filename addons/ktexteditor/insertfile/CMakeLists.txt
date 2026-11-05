set(ktexteditor_insertfile_PART_SRCS insertfileplugin.cpp )

kate_add_static_editor_plugin(ktexteditor_insertfile ${ktexteditor_insertfile_PART_SRCS})

target_link_libraries(ktexteditor_insertfile
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(services FILES ktexteditor_insertfile.desktop)
kate_install_data(ktexteditor_insertfile FILES ktexteditor_insertfileui.rc)
