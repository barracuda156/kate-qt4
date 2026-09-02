# qt4compat: actions, XMLGUI, main window classes and KParts (agent B)

list(APPEND QT4COMPAT_GUI_SRCS
    src/gui/actions/kshortcut.cpp
    src/gui/actions/kstandardshortcut.cpp
    src/gui/actions/kaction.cpp
    src/gui/actions/ktoggleaction.cpp
    src/gui/actions/kactionmenu.cpp
    src/gui/actions/ktogglefullscreenaction.cpp
    src/gui/actions/kpastetextaction.cpp
    src/gui/actions/ktoggletoolbaraction.cpp
    src/gui/actions/kselectaction.cpp
    src/gui/actions/krecentfilesaction.cpp
    src/gui/actions/kstandardaction.cpp
    src/gui/actions/kactioncollection.cpp
)

list(APPEND QT4COMPAT_XMLGUI_SRCS
    src/xmlgui/kmenu.cpp
    src/xmlgui/kmenubar.cpp
    src/xmlgui/kstatusbar.cpp
    src/xmlgui/ktoolbar.cpp
    src/xmlgui/kmainwindow.cpp
    src/xmlgui/kxmlguiwindow.cpp
    src/xmlgui/kxmlguiclient.cpp
    src/xmlgui/kxmlguifactory.cpp
    src/xmlgui/kxmlguifactory_p.cpp
    src/xmlgui/kxmlguibuilder.cpp
    src/xmlgui/kxmlguiversionhandler.cpp
    src/xmlgui/ktoolbarhandler.cpp
    src/xmlgui/khelpmenu.cpp
    src/xmlgui/kedittoolbar.cpp
    src/xmlgui/kkeysequencewidget.cpp
    src/xmlgui/kshortcutseditor.cpp
    src/xmlgui/kshortcutsdialog.cpp
)

list(APPEND QT4COMPAT_PARTS_SRCS
    src/parts/part.cpp
    src/parts/mainwindow.cpp
    src/parts/partmanager.cpp
    src/parts/factory.cpp
    src/parts/event.cpp
    src/parts/browserextension.cpp
    src/parts/plugin.cpp
)

list(APPEND QT4COMPAT_MOC_HEADERS
    include/kaction.h
    include/ktoggleaction.h
    include/kactionmenu.h
    include/ktogglefullscreenaction.h
    include/kpastetextaction.h
    include/ktoggletoolbaraction.h
    include/kselectaction.h
    include/krecentfilesaction.h
    include/kstandardaction_p.h
    include/kactioncollection.h
    include/kmenu.h
    include/kmenubar.h
    include/kstatusbar.h
    include/ktoolbar.h
    include/kmainwindow.h
    include/kxmlguiwindow.h
    include/kxmlguifactory.h
    include/khelpmenu.h
    include/kedittoolbar.h
    include/kkeysequencewidget.h
    include/kshortcutseditor.h
    include/kshortcutsdialog.h
    include/kparts/part.h
    include/kparts/mainwindow.h
    include/kparts/partmanager.h
    include/kparts/factory.h
    include/kparts/browserextension.h
    include/kparts/plugin.h
    src/xmlgui/ktoolbarhandler_p.h
)

list(APPEND QT4COMPAT_QRC_FILES
    data/xmlgui.qrc
)
