set(ktexteditor_hlselection_PART_SRCS hlselectionplugin.cpp )

kate_add_static_editor_plugin(ktexteditor_hlselection ${ktexteditor_hlselection_PART_SRCS})

target_link_libraries(ktexteditor_hlselection
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(services FILES ktexteditor_hlselection.desktop)

# Dominik: right now, we do not have any actions, so do not insall ui.rc file
# kate_install_data(ktexteditor_hlselection FILES ktexteditor_hlselectionui.rc)
