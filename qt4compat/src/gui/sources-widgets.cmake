# qt4compat: widgets, dialogs and icon theme (agent C)
list(APPEND QT4COMPAT_GUI_SRCS
    src/gui/widgets/kguiitem.cpp
    src/gui/widgets/kstandardguiitem.cpp
    src/gui/widgets/kpushbutton.cpp
    src/gui/widgets/khbox.cpp
    src/gui/widgets/kvbox.cpp
    src/gui/widgets/kseparator.cpp
    src/gui/widgets/klistwidget.cpp
    src/gui/widgets/kallocator.cpp
    src/gui/widgets/kcompletion.cpp
    src/gui/widgets/kcompletionbase.cpp
    src/gui/widgets/kcompletionbox.cpp
    src/gui/widgets/klineedit.cpp
    src/gui/widgets/kcombobox.cpp
    src/gui/widgets/ktabbar.cpp
    src/gui/widgets/ktabwidget.cpp
    src/gui/widgets/ktextedit.cpp
    src/gui/widgets/ksqueezedtextlabel.cpp
    src/gui/widgets/knuminput.cpp
    src/gui/widgets/kcolorbutton.cpp
    src/gui/widgets/kcolorcombo.cpp
    src/gui/widgets/fonthelpers.cpp
    src/gui/widgets/kfontchooser.cpp
    src/gui/widgets/kfontcombobox.cpp
    src/gui/widgets/ktitlewidget.cpp
    src/gui/widgets/kmultitabbar.cpp
    src/gui/widgets/kmessagewidget.cpp
    src/gui/widgets/kcategorizedview.cpp
    src/gui/widgets/kcategorydrawer.cpp
    src/gui/widgets/kcategorizedsortfilterproxymodel.cpp
    src/gui/widgets/kicontheme.cpp
    src/gui/widgets/kcursor.cpp
    src/gui/widgets/kwindowsystem.cpp
    src/gui/dialogs/kdialog.cpp
    src/gui/dialogs/kmessagebox.cpp
    src/gui/dialogs/kinputdialog.cpp
    src/gui/dialogs/kprogressdialog.cpp
    src/gui/dialogs/kcolordialog.cpp
    src/gui/dialogs/kfontdialog.cpp
    src/gui/dialogs/kpagedialog.cpp
    src/gui/dialogs/kpagemodel.cpp
    src/gui/dialogs/kpageview.cpp
    src/gui/dialogs/kpageview_p.cpp
    src/gui/dialogs/kpagewidget.cpp
    src/gui/dialogs/kpagewidgetmodel.cpp
    src/gui/dialogs/kfiledialog.cpp
    src/gui/dialogs/kencodingfiledialog.cpp
    src/gui/dialogs/kmimetypechooser.cpp
    src/gui/dialogs/kaboutapplicationdialog.cpp
    src/gui/dialogs/kopenwithdialog.cpp
    src/gui/dialogs/kicondialog.cpp
    src/gui/dialogs/ktip.cpp
)

list(APPEND QT4COMPAT_MOC_HEADERS
    include/kpushbutton.h
    include/klineedit.h
    include/kcombobox.h
    include/ktextedit.h
    include/kcolorbutton.h
    include/kwindowsystem.h
    include/kencodingfiledialog.h
    include/kmimetypechooser.h
    include/kaboutapplicationdialog.h
    include/kopenwithdialog.h
    include/kicondialog.h
)

# vendored kdelibs classes use Q_PRIVATE_SLOT, so their moc output must be
# included from the .cpp that defines the private class: generate <name>.moc
# into the build directory (which is on the include path)
set(_qt4compat_widgets_moc_pairs
    "include/kcategorizedview.h;kcategorizedview.moc"
    "include/kcategorydrawer.h;kcategorydrawer.moc"
    "include/kcolorcombo.h;kcolorcombo.moc"
    "include/kcompletion.h;kcompletion.moc"
    "include/kcompletionbox.h;kcompletionbox.moc"
    "include/kdialog.h;kdialog.moc"
    "src/gui/dialogs/kdialogqueue_p.h;kdialogqueue_p.moc"
    "include/kfontchooser.h;kfontchooser.moc"
    "include/kfontcombobox.h;kfontcombobox.moc"
    "include/kfontdialog.h;kfontdialog.moc"
    "include/khbox.h;khbox.moc"
    "src/gui/dialogs/kinputdialog_p.h;kinputdialog_p.moc"
    "include/klistwidget.h;klistwidget.moc"
    "include/kmessagewidget.h;kmessagewidget.moc"
    "include/kmultitabbar.h;kmultitabbar.moc"
    "src/gui/widgets/kmultitabbar_p.h;kmultitabbar_p.moc"
    "include/knuminput.h;knuminput.moc"
    "include/kpagedialog.h;kpagedialog.moc"
    "include/kpagemodel.h;kpagemodel.moc"
    "include/kpageview.h;kpageview.moc"
    "src/gui/dialogs/kpageview_p.h;kpageview_p.moc"
    "include/kpagewidget.h;kpagewidget.moc"
    "include/kpagewidgetmodel.h;kpagewidgetmodel.moc"
    "include/kprogressdialog.h;kprogressdialog.moc"
    "include/kseparator.h;kseparator.moc"
    "include/ksqueezedtextlabel.h;ksqueezedtextlabel.moc"
    "include/ktabbar.h;ktabbar.moc"
    "include/ktabwidget.h;ktabwidget.moc"
    "include/ktitlewidget.h;ktitlewidget.moc"
    "include/kvbox.h;kvbox.moc"
    "src/gui/widgets/sampleedit_p.h;sampleedit_p.moc"
)
foreach(_pair ${_qt4compat_widgets_moc_pairs})
    list(GET _pair 0 _hdr)
    list(GET _pair 1 _out)
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/${_hdr} ${CMAKE_CURRENT_BINARY_DIR}/${_out})
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${_out} PROPERTIES HEADER_FILE_ONLY TRUE GENERATED TRUE)
    list(APPEND QT4COMPAT_GUI_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${_out})
endforeach()

list(APPEND QT4COMPAT_QRC_FILES data/icons.qrc)
