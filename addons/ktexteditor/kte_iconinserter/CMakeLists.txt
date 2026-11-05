set(ktexteditor_iconinserter_SRCS
	iconinserterplugin.cpp
)

kate_add_static_editor_plugin(ktexteditor_iconinserter ${ktexteditor_iconinserter_SRCS})

target_link_libraries(ktexteditor_iconinserter
	ktexteditor
	qt4compat
	${QT_QTGUI_LIBRARY}
	${QT_QTCORE_LIBRARY}
)

kate_install_data(services FILES ktexteditor_iconinserter.desktop)
kate_install_data(ktexteditor_iconinserter FILES ktexteditor_iconinserterui.rc)
