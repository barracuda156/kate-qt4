set(ktexteditor_exporter_PART_SRCS
  exporterplugin.cpp
  exporterpluginview.cpp
  htmlexporter.cpp
)

kate_add_static_editor_plugin(ktexteditor_exporter ${ktexteditor_exporter_PART_SRCS})

target_link_libraries(ktexteditor_exporter
    ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)

kate_install_data(ktexteditor_exporter FILES ktexteditor_exporterui.rc)
kate_install_data(services FILES ktexteditor_exporter.desktop)
