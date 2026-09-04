# qt4compat: pieces added during integration
list(APPEND QT4COMPAT_GUI_SRCS src/gui/widgets/kshortcutwidget.cpp)
list(APPEND QT4COMPAT_MOC_HEADERS include/kshortcutwidget.h)

# KAcceleratorManager (vendored from kdelibs kdeui/shortcuts/), needed by the
# search plugin (addons/kate/search). Its private header declares Q_OBJECT
# classes (KPopupAccelManager, QWidgetStackAccelManager) that are not part of
# the public API, so - like the kdialogqueue_p.h / kinputdialog_p.h pairs in
# sources-widgets.cmake - it is moc'ed into a <name>.moc the .cpp includes,
# instead of being listed in QT4COMPAT_MOC_HEADERS.
list(APPEND QT4COMPAT_GUI_SRCS src/gui/shortcuts/kacceleratormanager.cpp)
set(_qt4compat_extra_moc_pairs
    "src/gui/shortcuts/kacceleratormanager_private.h\;kacceleratormanager_private.moc"
)
foreach(_pair ${_qt4compat_extra_moc_pairs})
    list(GET _pair 0 _hdr)
    list(GET _pair 1 _out)
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/${_hdr} ${CMAKE_CURRENT_BINARY_DIR}/${_out})
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${_out} PROPERTIES HEADER_FILE_ONLY TRUE GENERATED TRUE)
    list(APPEND QT4COMPAT_GUI_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${_out})
endforeach()

# KHistoryComboBox and KUrlRequester/KUrlComboRequester: trimmed local
# replacements (no KIO) for widgets several plugins' .ui files promote to,
# needed to bring up addons/kate/search (search.ui: searchCombo/replaceCombo
# are KHistoryComboBox, folderRequester is a KUrlComboRequester).
list(APPEND QT4COMPAT_GUI_SRCS
    src/gui/widgets/khistorycombobox.cpp
    src/gui/widgets/kurlrequester.cpp
)
list(APPEND QT4COMPAT_MOC_HEADERS
    include/khistorycombobox.h
    include/kurlrequester.h
)
