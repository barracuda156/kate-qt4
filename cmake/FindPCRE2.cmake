# FindPCRE2.cmake - the 16-bit pcre2 library (libpcre2-16), KatePart's regex
# engine (part/utils/katepcre.cpp). pkg-config first (libpcre2-16), then a
# plain header/library search.
#
# Defines PCRE2_FOUND, PCRE2_INCLUDE_DIRS, PCRE2_LIBRARIES, PCRE2_VERSION.

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_PCRE2 QUIET libpcre2-16)
endif()

find_path(PCRE2_INCLUDE_DIR pcre2.h
    HINTS ${PC_PCRE2_INCLUDEDIR} ${PC_PCRE2_INCLUDE_DIRS})
find_library(PCRE2_16_LIBRARY NAMES pcre2-16
    HINTS ${PC_PCRE2_LIBDIR} ${PC_PCRE2_LIBRARY_DIRS})

set(PCRE2_VERSION ${PC_PCRE2_VERSION})
if(NOT PCRE2_VERSION AND PCRE2_INCLUDE_DIR AND EXISTS "${PCRE2_INCLUDE_DIR}/pcre2.h")
    file(STRINGS "${PCRE2_INCLUDE_DIR}/pcre2.h" _pcre2_major REGEX "^#define PCRE2_MAJOR +[0-9]+")
    file(STRINGS "${PCRE2_INCLUDE_DIR}/pcre2.h" _pcre2_minor REGEX "^#define PCRE2_MINOR +[0-9]+")
    string(REGEX REPLACE "^#define PCRE2_MAJOR +([0-9]+).*" "\\1" _pcre2_major "${_pcre2_major}")
    string(REGEX REPLACE "^#define PCRE2_MINOR +([0-9]+).*" "\\1" _pcre2_minor "${_pcre2_minor}")
    if(_pcre2_major AND _pcre2_minor)
        set(PCRE2_VERSION "${_pcre2_major}.${_pcre2_minor}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCRE2
    REQUIRED_VARS PCRE2_16_LIBRARY PCRE2_INCLUDE_DIR
    VERSION_VAR PCRE2_VERSION)

if(PCRE2_FOUND)
    set(PCRE2_LIBRARIES ${PCRE2_16_LIBRARY})
    set(PCRE2_INCLUDE_DIRS ${PCRE2_INCLUDE_DIR})
endif()

mark_as_advanced(PCRE2_INCLUDE_DIR PCRE2_16_LIBRARY)
