# qt4compat: sources added by the text/IO/services/Sonnet part of the layer
# (relative to qt4compat/)

file(GLOB _qt4compat_prober_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/io/probers/*.cpp)
list(SORT _qt4compat_prober_srcs)

list(APPEND QT4COMPAT_IO_SRCS
    ${_qt4compat_prober_srcs}
    src/io/kencodingprober.cpp
    src/io/kcharsets.cpp
    src/io/kfilterbase.cpp
    src/io/kfilterdev.cpp
    src/io/kgzipfilter.cpp
    src/io/kbzip2filter.cpp
    src/io/kmimetype.cpp
    src/io/kservice.cpp
    src/io/kservicetypetrader.cpp
    src/io/kmimetypetrader.cpp
    src/io/kplugininfo.cpp
    src/io/kpluginfactory.cpp
    src/io/kpluginloader.cpp
    src/io/klibrary.cpp
    src/io/klibloader.cpp
    src/io/kio_netaccess.cpp
    src/io/kfileitem.cpp
    src/io/krun.cpp
    src/io/kshell.cpp
    src/io/kshell_unix.cpp
    src/io/kprocess.cpp
    src/io/kcodecs.cpp
    src/io/kurlcompletion.cpp
)

list(APPEND QT4COMPAT_SONNET_SRCS
    src/sonnet/speller.cpp
    src/sonnet/backgroundchecker.cpp
    src/sonnet/dictionarycombobox.cpp
    src/sonnet/dialog.cpp
    src/sonnet/configwidget.cpp
)

list(APPEND QT4COMPAT_MOC_HEADERS
    include/kprocess.h
    include/kpluginfactory.h
    include/krun.h
    include/sonnet/backgroundchecker.h
    include/sonnet/dictionarycombobox.h
    include/sonnet/dialog.h
    include/sonnet/configwidget.h
    include/sonnet/configdialog.h
)

# probers and their tables include each other relatively
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/io)

if(BZIP2_FOUND)
    set_property(SOURCE src/io/kfilterbase.cpp src/io/kbzip2filter.cpp APPEND PROPERTY COMPILE_DEFINITIONS HAVE_BZIP2=1)
    include_directories(${BZIP2_INCLUDE_DIR})
    # FindBZip2.cmake already probes whether bzlib only exports the
    # BZ2_-prefixed symbols (every bzlib since 1.0 does); wire its result
    # into the macro kbzip2filter.cpp checks to pick the right names.
    if(BZIP2_NEED_PREFIX)
        set_property(SOURCE src/io/kbzip2filter.cpp APPEND PROPERTY COMPILE_DEFINITIONS NEED_BZ2_PREFIX=1)
    endif()
endif()
