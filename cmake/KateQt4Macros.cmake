# KateQt4Macros.cmake - helpers for building Kate against plain Qt 4
#
# kate_automoc(<outvar> <src.cpp>...)
#   Emulates KDE4's automoc4 with the vendored Qt4Macros:
#   * for every `#include "X.moc"` (or <X.moc>) in a source, moc is run on
#     X.h if that header exists and declares Q_OBJECT/Q_GADGET, otherwise on
#     X.cpp if it declares Q_OBJECT, otherwise on X.h; the output is
#     ${CMAKE_CURRENT_BINARY_DIR}/X.moc (the binary dir is added to the
#     include path, so the include resolves; base names must be unique per
#     directory, which holds for part/ and ktexteditor/);
#   * for every foo.cpp whose sibling foo.h declares Q_OBJECT but which does
#     not include "foo.moc", qt4_wrap_cpp(foo.h) is run and moc_foo.cxx is
#     appended to <outvar> so it gets compiled;
#   Generated files are appended to <outvar>; add them to the target sources.
#
# kate_automoc_headers(<outvar> <hdr.h>...)
#   qt4_wrap_cpp for headers that have no matching .cpp in the source list.
#
# kate_add_ui_files(<outvar> <file.ui>...)
#   qt4_wrap_ui wrapper; the ui_*.h files land in ${CMAKE_CURRENT_BINARY_DIR}.
#
# kate_install_data(<resource-relative-dir> FILES <file>... [DIRECTORY <dir> [PATTERN <glob>]])
#   Registers data files (XMLGUI .rc, syntax XML, scripts, config defaults)
#   under a directory relative to the "data" resource root that
#   KStandardDirs searches, e.g. kate_install_data(katepart FILES katepartui.rc)
#   makes KStandardDirs::locate("data", "katepart/katepartui.rc") work.
#   Layout: <root>/<dir>/<file>, where <root> is
#     * ${CMAKE_BINARY_DIR}/share                      when running from the build tree
#       (the root CMakeLists passes it as KATE_SOURCE_DATADIRS),
#     * <bundle>/Contents/Resources                    on macOS after install,
#     * ${CMAKE_INSTALL_PREFIX}/share/kate-qt4         elsewhere after install.
#   Files are copied into the staging tree at build time (target kate-data)
#   and installed with install(FILES)/install(DIRECTORY). Use "config" as
#   <dir> for files that KStandardDirs looks up with the "config" resource
#   type (they are placed in <root>/config/).
#   DIRECTORY installs a whole directory tree (optionally filtered by PATTERN,
#   e.g. "*.js"); its contents are copied recursively into <root>/<dir>/.
#   Files may be given as absolute paths (e.g. generated in the binary dir).

include(CMakeParseArguments)

if(NOT TARGET kate-data)
    add_custom_target(kate-data ALL)
endif()

set(KATE_DATA_STAGING_DIR "${CMAKE_BINARY_DIR}/share")

function(_kate_file_has_qobject _file _result)
    set(${_result} FALSE PARENT_SCOPE)
    if(EXISTS "${_file}")
        file(STRINGS "${_file}" _lines REGEX "^[ \t]*(Q_OBJECT|Q_GADGET)")
        if(_lines)
            set(${_result} TRUE PARENT_SCOPE)
        endif()
    endif()
endfunction()

macro(kate_automoc _outvar)
    foreach(_src ${ARGN})
        get_filename_component(_abs_src "${_src}" ABSOLUTE)
        get_filename_component(_src_dir "${_abs_src}" PATH)
        get_filename_component(_src_base "${_abs_src}" NAME_WE)
        set(_includes_own_moc FALSE)
        if(EXISTS "${_abs_src}")
            file(STRINGS "${_abs_src}" _moc_lines REGEX "^[ \t]*#[ \t]*include[ \t]*[<\"][A-Za-z0-9_]+\\.moc[>\"]")
            foreach(_line ${_moc_lines})
                string(REGEX REPLACE "^[ \t]*#[ \t]*include[ \t]*[<\"]([A-Za-z0-9_]+)\\.moc[>\"].*$" "\\1" _moc_base "${_line}")
                if(_moc_base STREQUAL _src_base)
                    set(_includes_own_moc TRUE)
                endif()
                set(_moc_out "${CMAKE_CURRENT_BINARY_DIR}/${_moc_base}.moc")
                list(FIND ${_outvar} "${_moc_out}" _already)
                if(_already EQUAL -1)
                    set(_hdr "${_src_dir}/${_moc_base}.h")
                    set(_cpp "${_src_dir}/${_moc_base}.cpp")
                    _kate_file_has_qobject("${_hdr}" _hdr_qobj)
                    _kate_file_has_qobject("${_cpp}" _cpp_qobj)
                    set(_moc_in "")
                    if(EXISTS "${_hdr}" AND _hdr_qobj)
                        set(_moc_in "${_hdr}")
                    elseif(EXISTS "${_cpp}" AND _cpp_qobj)
                        set(_moc_in "${_cpp}")
                    elseif(EXISTS "${_hdr}")
                        set(_moc_in "${_hdr}")
                    elseif(EXISTS "${_cpp}")
                        set(_moc_in "${_cpp}")
                    endif()
                    if(_moc_in)
                        qt4_generate_moc("${_moc_in}" "${_moc_out}")
                        list(APPEND ${_outvar} "${_moc_out}")
                    else()
                        message(WARNING "kate_automoc: no source found for ${_moc_base}.moc included by ${_src}")
                    endif()
                endif()
            endforeach()
        endif()
        # header with Q_OBJECT whose moc is not included by the .cpp
        set(_own_hdr "${_src_dir}/${_src_base}.h")
        if(NOT _includes_own_moc AND EXISTS "${_own_hdr}")
            _kate_file_has_qobject("${_own_hdr}" _own_qobj)
            if(_own_qobj)
                # make sure nobody else includes "<base>.moc" for this header
                set(_moc_out "${CMAKE_CURRENT_BINARY_DIR}/${_src_base}.moc")
                list(FIND ${_outvar} "${_moc_out}" _already)
                if(_already EQUAL -1)
                    set(_wrapped)
                    qt4_wrap_cpp(_wrapped "${_own_hdr}")
                    list(APPEND ${_outvar} ${_wrapped})
                endif()
            endif()
        endif()
    endforeach()
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro()

macro(kate_automoc_headers _outvar)
    foreach(_hdr ${ARGN})
        get_filename_component(_abs_hdr "${_hdr}" ABSOLUTE)
        _kate_file_has_qobject("${_abs_hdr}" _has)
        if(_has)
            set(_wrapped)
            qt4_wrap_cpp(_wrapped "${_abs_hdr}")
            list(APPEND ${_outvar} ${_wrapped})
        endif()
    endforeach()
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro()

macro(kate_add_ui_files _outvar)
    set(_ui_out)
    qt4_wrap_ui(_ui_out ${ARGN})
    list(APPEND ${_outvar} ${_ui_out})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro()

function(kate_install_data _dir)
    cmake_parse_arguments(KID "" "DIRECTORY;PATTERN" "FILES" ${ARGN})
    set(_staging "${KATE_DATA_STAGING_DIR}/${_dir}")
    if(APPLE)
        set(_install_dest "${KATE_BUNDLE_RESOURCES_INSTALL_DIR}/${_dir}")
    else()
        set(_install_dest "${KATE_DATA_INSTALL_DIR}/${_dir}")
    endif()
    string(REPLACE "/" "_" _tag "${_dir}")
    foreach(_f ${KID_FILES})
        get_filename_component(_abs "${_f}" ABSOLUTE)
        get_filename_component(_name "${_abs}" NAME)
        add_custom_command(OUTPUT "${_staging}/${_name}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_staging}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_abs}" "${_staging}/${_name}"
            DEPENDS "${_abs}"
            COMMENT "Staging ${_dir}/${_name}")
        string(MAKE_C_IDENTIFIER "kate-data-${_tag}-${_name}" _tname)
        add_custom_target(${_tname} DEPENDS "${_staging}/${_name}")
        add_dependencies(kate-data ${_tname})
        install(FILES "${_abs}" DESTINATION "${_install_dest}")
    endforeach()
    if(KID_DIRECTORY)
        get_filename_component(_abs_dir "${KID_DIRECTORY}" ABSOLUTE)
        string(MAKE_C_IDENTIFIER "kate-data-${_tag}-dir" _tname)
        if(KID_PATTERN)
            file(GLOB_RECURSE _dir_files RELATIVE "${_abs_dir}" "${_abs_dir}/${KID_PATTERN}")
        else()
            file(GLOB_RECURSE _dir_files RELATIVE "${_abs_dir}" "${_abs_dir}/*")
        endif()
        set(_staged_files)
        foreach(_rel ${_dir_files})
            get_filename_component(_rel_dir "${_rel}" PATH)
            add_custom_command(OUTPUT "${_staging}/${_rel}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${_staging}/${_rel_dir}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_abs_dir}/${_rel}" "${_staging}/${_rel}"
                DEPENDS "${_abs_dir}/${_rel}"
                COMMENT "Staging ${_dir}/${_rel}")
            list(APPEND _staged_files "${_staging}/${_rel}")
        endforeach()
        add_custom_target(${_tname} DEPENDS ${_staged_files})
        add_dependencies(kate-data ${_tname})
        if(KID_PATTERN)
            install(DIRECTORY "${_abs_dir}/" DESTINATION "${_install_dest}" FILES_MATCHING PATTERN "${KID_PATTERN}")
        else()
            install(DIRECTORY "${_abs_dir}/" DESTINATION "${_install_dest}")
        endif()
    endif()
endfunction()
