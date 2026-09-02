# qt4compat: kdelibs replacement on plain Qt 4 for Kate
#
# Everything here is compiled into one static library. Sections are grouped by
# the sub-projects they came from; the *_MOC_HEADERS lists must be kept in
# sync with headers containing Q_OBJECT.

set(QT4COMPAT_CORE_SRCS
    src/core/kdebug.cpp
    src/core/klocalizedstring.cpp
    src/core/klocale.cpp
    src/core/kglobal.cpp
    src/core/kcomponentdata.cpp
    src/core/kstandarddirs.cpp
    src/core/kaboutdata.cpp
    src/core/kcmdlineargs.cpp
    src/core/kurl.cpp
    src/core/knotification.cpp
    src/core/ktoolinvocation.cpp
    src/core/kuser.cpp
)

set(QT4COMPAT_IO_SRCS
    src/io/ktemporaryfile.cpp
    src/io/ksavefile.cpp
    src/io/kdirwatch.cpp
)

set(QT4COMPAT_GUI_SRCS
    src/gui/kglobalsettings.cpp
    src/gui/kapplication.cpp
    src/gui/kicon.cpp
    src/gui/kiconloader.cpp
)

# filled in by the config / xmlgui / parts / sonnet parts of the layer
set(QT4COMPAT_CONFIG_SRCS)
set(QT4COMPAT_XMLGUI_SRCS)
set(QT4COMPAT_PARTS_SRCS)
set(QT4COMPAT_SONNET_SRCS)

set(QT4COMPAT_MOC_HEADERS
    include/kglobalsettings.h
    include/kapplication.h
    include/kdirwatch.h
)

set(QT4COMPAT_UI_FILES)
set(QT4COMPAT_QRC_FILES)

# optional pieces of the layer: every src/<dir>/sources*.cmake appends to the
# QT4COMPAT_*_SRCS / QT4COMPAT_MOC_HEADERS / QT4COMPAT_UI_FILES / QT4COMPAT_QRC_FILES lists
file(GLOB _qt4compat_source_lists ${CMAKE_CURRENT_SOURCE_DIR}/src/*/sources*.cmake)
list(SORT _qt4compat_source_lists)
foreach(_list ${_qt4compat_source_lists})
    include(${_list})
endforeach()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

qt4_wrap_cpp(QT4COMPAT_MOC_SRCS ${QT4COMPAT_MOC_HEADERS})
if(QT4COMPAT_UI_FILES)
    qt4_wrap_ui(QT4COMPAT_UI_SRCS ${QT4COMPAT_UI_FILES})
endif()
if(QT4COMPAT_QRC_FILES)
    qt4_add_resources(QT4COMPAT_QRC_SRCS ${QT4COMPAT_QRC_FILES})
endif()

add_library(qt4compat STATIC
    ${QT4COMPAT_CORE_SRCS}
    ${QT4COMPAT_IO_SRCS}
    ${QT4COMPAT_GUI_SRCS}
    ${QT4COMPAT_CONFIG_SRCS}
    ${QT4COMPAT_XMLGUI_SRCS}
    ${QT4COMPAT_PARTS_SRCS}
    ${QT4COMPAT_SONNET_SRCS}
    ${QT4COMPAT_MOC_SRCS}
    ${QT4COMPAT_UI_SRCS}
    ${QT4COMPAT_QRC_SRCS}
)

target_link_libraries(qt4compat
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${ZLIB_LIBRARIES}
)
if(BZIP2_FOUND)
    target_link_libraries(qt4compat ${BZIP2_LIBRARIES})
endif()
