# KTextEditor interfaces (static library, pure Qt 4)

set(ktexteditor_LIB_SRCS
   ktexteditor.cpp
   document.cpp
   range.cpp
   cursor.cpp
   templateinterface.cpp
   templateinterface2.cpp
   editorchooser.cpp
   attribute.cpp
   cursorfeedback.cpp
   rangefeedback.cpp
   smartcursor.cpp
   smartrange.cpp
   codecompletioninterface.cpp
   codecompletionmodel.cpp
   codecompletionmodelcontrollerinterface.cpp
   configinterface.cpp
   smartinterface.cpp
   highlightinterface.cpp
   movinginterface.cpp
   movingcursor.cpp
   movingrange.cpp
   movingrangefeedback.cpp
   messageinterface.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

kate_add_ui_files(ktexteditor_LIB_SRCS editorchooser_ui.ui)
kate_automoc(ktexteditor_LIB_SRCS ${ktexteditor_LIB_SRCS})

add_library(ktexteditor STATIC ${ktexteditor_LIB_SRCS})

target_link_libraries(ktexteditor
    qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTCORE_LIBRARY}
)
