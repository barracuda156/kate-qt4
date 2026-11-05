# Kate and KWrite on plain Qt 4 (no kdelibs) - see BUILDING-QT4.md and qt4compat/README.md
cmake_minimum_required(VERSION 3.5)
project(kate)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(CheckFunctionExists)
include(KateQt4Macros)

set(KATE_VERSION 4.14.3)

option(KATE_BUILD_KWRITE "Build the KWrite editor shell" ON)
option(KATE_BUILD_KATE "Build the Kate MDI application" ON)
option(KATE_BUILD_ADDONS "Build the ported addons: KatePart editor plugins for KWrite and Kate, Kate application plugins (see addons/)" ON)
option(BUILD_TESTING "Build tests (not ported yet)" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# The sources are C++98. Build them as gnu++98: that keeps the 'register'
# keyword legal on every compiler (a hard error in the C++17 default of new
# GCCs, which is the only reason C++11 was requested before) WITHOUT demanding
# a C++11 compiler. Requiring C++11 forced MacPorts to pick a newer compiler
# than Qt 4 was built with (macports-gcc on 10.6 PowerPC, where Apple gcc has
# no C++11): its libstdc++ and the system libstdc++ under Qt were then both
# loaded, and mixing two C++ runtimes in one process corrupted the heap - Kate
# crashed dereferencing a bogus QSharedData pointer at first view creation.
# gnu++98 lets the port use the same compiler, hence the same C++ runtime, as
# Qt 4 (Apple gcc on 10.6, clang on 10.15).
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=gnu++98" KATE_HAS_STD_GNUXX98)
if(KATE_HAS_STD_GNUXX98)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++98")
endif()

# Qt 4.8 (vendored FindQt4.cmake in cmake/, CMake 4 dropped it)
set(QT_USE_IMPORTED_TARGETS OFF)
set(QT_MIN_VERSION 4.8.0)
find_package(Qt4 4.8.0 REQUIRED QtCore QtGui QtXml QtScript QtNetwork)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSCRIPT TRUE)
set(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})

find_package(ZLIB REQUIRED)
find_package(BZip2)
if(BZIP2_FOUND)
    add_definitions(-DHAVE_BZIP2=1)
    include_directories(${BZIP2_INCLUDE_DIR})
endif()
# Perl and Python 3 run upstream's generators for the derived syntax
# highlighting definitions (part/syntax/generators) at build time
find_package(Perl REQUIRED)
find_program(PYTHON_EXECUTABLE NAMES python3 python3.14 python3.13 python3.12 python3.11 python3.10 python3.9 python)
if(NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python 3 not found; it generates the Jinja syntax highlighting variants (set PYTHON_EXECUTABLE)")
endif()

# pcre2 (16-bit) is the regex engine of syntax highlighting and search.
# 10.43 added variable-length lookbehind, which a few upstream highlighting
# definitions use (fish.xml); an older library only loses those rules.
find_package(PCRE2 REQUIRED)
if(PCRE2_VERSION AND PCRE2_VERSION VERSION_LESS 10.43)
    message(WARNING "pcre2 ${PCRE2_VERSION} found; 10.43 or newer is recommended (variable-length lookbehind in some highlighting definitions)")
endif()

# config.h
if(NOT APPLE)
    check_function_exists(fdatasync HAVE_FDATASYNC)
    check_function_exists(ctermid HAVE_CTERMID)
endif()
configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

# install layout
if(APPLE)
    set(KATE_INSTALL_BUNDLE_DIR "/Applications" CACHE PATH "Where .app bundles are installed")
    # Every application bundle is self-contained: kate_install_data() installs
    # the data files into Contents/Resources of each bundle listed here (where
    # KStandardDirs looks first), see KateQt4Macros.cmake.
    set(KATE_APP_BUNDLES)
    if(KATE_BUILD_KWRITE)
        list(APPEND KATE_APP_BUNDLES kwrite)
        set(KATE_APP_kwrite_RESOURCES_DIR "${KATE_INSTALL_BUNDLE_DIR}/KWrite.app/Contents/Resources")
    endif()
    if(KATE_BUILD_KATE)
        list(APPEND KATE_APP_BUNDLES kate)
        set(KATE_APP_kate_RESOURCES_DIR "${KATE_INSTALL_BUNDLE_DIR}/Kate.app/Contents/Resources")
    endif()
else()
    set(KATE_INSTALL_BUNDLE_DIR "" CACHE PATH "Unused on this platform")
endif()
set(KATE_DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/kate-qt4")
set(KATE_BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")

# definitions shared by every target
add_definitions(
    -DQT_USE_FAST_CONCATENATION
    -DQT_USE_FAST_OPERATOR_PLUS
    -DKTEXTEDITOR_NO_DEPRECATED
    -DKATE_INSTALL_DATADIR="${KATE_DATA_INSTALL_DIR}"
    -DKATE_SOURCE_DATADIRS="${KATE_DATA_STAGING_DIR}"
)
if(NOT APPLE)
    add_definitions(-D_LARGEFILE64_SOURCE)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/qt4compat/include
    ${CMAKE_SOURCE_DIR}/qt4compat/include/KDE
    ${QT_INCLUDES}
    ${ZLIB_INCLUDE_DIRS}
    ${PCRE2_INCLUDE_DIRS}
)

add_subdirectory(qt4compat)
add_subdirectory(ktexteditor)
add_subdirectory(part)
# the addons are static plugins linked into the executables; they register
# themselves (kate_add_static_plugin, kate_add_static_editor_plugin) and must
# be seen before kwrite and kate/app generate their import lists and link them
if(KATE_BUILD_ADDONS)
    add_subdirectory(addons)
endif()
if(KATE_BUILD_KWRITE)
    add_subdirectory(kwrite)
endif()
if(KATE_BUILD_KATE)
    add_subdirectory(kate)
endif()
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
