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
# kate_install_data(<resource-relative-dir> FILES <file>... [DIRECTORY <dir> [PATTERN <glob>]]
#                   [APPS <app>...])
#   Registers data files (XMLGUI .rc, syntax XML, scripts, config defaults)
#   under a directory relative to the "data" resource root that
#   KStandardDirs searches, e.g. kate_install_data(katepart FILES katepartui.rc)
#   makes KStandardDirs::locate("data", "katepart/katepartui.rc") work.
#   Layout: <root>/<dir>/<file>, where <root> is
#     * ${CMAKE_BINARY_DIR}/share                      when running from the build tree
#       (the root CMakeLists passes it as KATE_SOURCE_DATADIRS),
#     * <bundle>/Contents/Resources                    on macOS after install,
#       for every application bundle being built (KATE_APP_BUNDLES, set by
#       the root CMakeLists: each .app carries its own copy of the data it
#       needs, so the bundles stay self-contained) - or only for the bundles
#       named with APPS, for files that belong to one application
#       (kate_install_data(kwrite FILES kwriteui.rc APPS kwrite)),
#     * ${CMAKE_INSTALL_PREFIX}/share/kate-qt4         elsewhere after install
#       (APPS is ignored there: one shared data directory).
#   Files are copied into the staging tree at build time (target kate-data)
#   and installed with install(FILES)/install(DIRECTORY). Use "config" as
#   <dir> for files that KStandardDirs looks up with the "config" resource
#   type (they are placed in <root>/config/), "services" for plugin .desktop
#   files (KService/KServiceTypeTrader scan <root>/services/).
#   DIRECTORY installs a whole directory tree (optionally filtered by PATTERN,
#   e.g. "*.js"); its contents are copied recursively into <root>/<dir>/.
#   Files may be given as absolute paths (e.g. generated in the binary dir).
#
# kate_add_static_plugin(<name> <src.cpp>...)
#   Builds a KDE-style plugin (K_PLUGIN_FACTORY/K_EXPORT_PLUGIN sources plus a
#   .desktop service file) as a static library that the kate executable links
#   in. <name> must be the X-KDE-Library value of the plugin's .desktop file.
#   The sources are compiled with KDE_STATIC_PLUGIN_NAME=<name>, which makes
#   K_EXPORT_PLUGIN() emit kde_static_plugin_create_<name>() (see
#   qt4compat/include/kexportplugin.h and kpluginloader.h). The name is
#   appended to the global property KATE_STATIC_PLUGINS; kate/app links all
#   of them and generates katebuiltinplugins.cpp with one K_IMPORT_PLUGIN()
#   per plugin, so a plugin directory only needs this call plus
#   target_link_libraries() for what it uses and
#   kate_install_data(services FILES <name>.desktop) (plus its ui.rc under
#   kate/plugins/<component>/). Plugin directories are processed before
#   kate/app (see the root CMakeLists.txt).
#
# kate_macos_app_icon(<outvar> <icon.icns> <icon.png>)
#   Sets <outvar> to the .icns source file to add to a MACOSX_BUNDLE target
#   (with MACOSX_PACKAGE_LOCATION Resources) and MACOSX_BUNDLE_ICON_FILE in
#   the calling scope: a prebuilt qt4compat/data/<icon.icns> is used when
#   present, otherwise the icon is generated at build time with sips
#   (available on every macOS since 10.4) from <icon.png>. <outvar> stays
#   empty when neither is possible.
#
# kate_set_bundle_properties(<target> <BundleName> <bundle-identifier> <info-string>)
#   The MACOSX_BUNDLE_* properties shared by the application bundles
#   (Info.plist template, version from KATE_VERSION, copyright).
#
# kate_link_whole_archive(<target> <static-lib>)
#   Links <static-lib> into <target> forcing every object file in, instead of
#   only the ones that resolve an otherwise-undefined symbol. KatePart
#   registers itself with KTextEditor::EditorChooser via a static object's
#   constructor (part/utils/kateglobal.cpp, KateEditorRegistrar) rather than
#   a symbol any caller references directly, since it plays the role a
#   dynamically loaded plugin would elsewhere (dlopen runs every static
#   initializer in a .so; a plain `target_link_libraries` against a static
#   archive does not, so the linker drops that translation unit and the
#   registration never happens, e.g. EditorChooser::editor() silently
#   returning NULL, "A KDE text-editor component could not be found").
#   Use this instead of a plain target_link_libraries() call for
#   katepartinterfaces in every executable that expects KatePart to
#   self-register (kwrite, and the kate app once it exists).
function(kate_link_whole_archive _target _lib)
    if(APPLE)
        target_link_libraries(${_target} -Wl,-force_load,$<TARGET_FILE:${_lib}>)
    elseif(MSVC)
        target_link_libraries(${_target} -WHOLEARCHIVE:$<TARGET_FILE:${_lib}>)
    else()
        target_link_libraries(${_target} -Wl,--whole-archive ${_lib} -Wl,--no-whole-archive)
    endif()
    # $<TARGET_FILE:...> embedded in a linker-flag string isn't parsed by
    # target_link_libraries() for build-order purposes the way a bare target
    # name is, so without this make can link _target before _lib exists:
    #   ld: file not found: .../libkatepartinterfaces.a
    add_dependencies(${_target} ${_lib})
endfunction()

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
        get_filename_component(_src_ext "${_src}" EXT)
        string(TOLOWER "${_src_ext}" _src_ext)
        # headers are sometimes listed alongside their .cpp in a target's
        # source list (e.g. for IDE visibility); kate_automoc only makes
        # sense for compiled sources (see the doc comment above,
        # "kate_automoc(<outvar> <src.cpp>...)") - a header reaching here
        # would be treated as its own _src, and "does this source's sibling
        # header need wrapping" would then wrap the header against itself,
        # duplicating whatever moc output its real .cpp already generates
        # (duplicate symbols at link time, only visible once something force-
        # links the whole static library instead of pulling in .o files on
        # demand - see kate_link_whole_archive()).
        if(NOT _src_ext MATCHES "\\.(cpp|cxx|cc|c\\+\\+)$")
            continue()
        endif()
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
    cmake_parse_arguments(KID "" "DIRECTORY;PATTERN" "FILES;APPS" ${ARGN})
    set(_staging "${KATE_DATA_STAGING_DIR}/${_dir}")
    set(_install_dests)
    if(APPLE)
        set(_apps ${KID_APPS})
        if(NOT _apps)
            set(_apps ${KATE_APP_BUNDLES})
        endif()
        foreach(_app ${_apps})
            if(KATE_APP_${_app}_RESOURCES_DIR)
                list(APPEND _install_dests "${KATE_APP_${_app}_RESOURCES_DIR}/${_dir}")
            endif()
        endforeach()
    else()
        set(_install_dests "${KATE_DATA_INSTALL_DIR}/${_dir}")
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
        foreach(_dest ${_install_dests})
            install(FILES "${_abs}" DESTINATION "${_dest}")
        endforeach()
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
        foreach(_dest ${_install_dests})
            if(KID_PATTERN)
                install(DIRECTORY "${_abs_dir}/" DESTINATION "${_dest}" FILES_MATCHING PATTERN "${KID_PATTERN}")
            else()
                install(DIRECTORY "${_abs_dir}/" DESTINATION "${_dest}")
            endif()
        endforeach()
    endif()
endfunction()

function(kate_add_static_plugin _name)
    set(_srcs ${ARGN})
    set(_mocs)
    kate_automoc(_mocs ${_srcs})
    add_library(${_name} STATIC ${_srcs} ${_mocs})
    set_property(TARGET ${_name} APPEND PROPERTY COMPILE_DEFINITIONS KDE_STATIC_PLUGIN_NAME=${_name})
    # the kate executable links every registered plugin and generates the
    # K_IMPORT_PLUGIN() list from this property (kate/app/CMakeLists.txt), so
    # plugin directories must be processed before kate/app
    set_property(GLOBAL APPEND PROPERTY KATE_STATIC_PLUGINS ${_name})
endfunction()

macro(kate_macos_app_icon _outvar _icns _png)
    set(${_outvar})
    if(EXISTS ${CMAKE_SOURCE_DIR}/qt4compat/data/${_icns})
        set(${_outvar} ${CMAKE_SOURCE_DIR}/qt4compat/data/${_icns})
    elseif(EXISTS ${_png})
        find_program(SIPS_EXECUTABLE sips)
        if(SIPS_EXECUTABLE)
            set(${_outvar} ${CMAKE_CURRENT_BINARY_DIR}/${_icns})
            add_custom_command(OUTPUT ${${_outvar}}
                COMMAND ${SIPS_EXECUTABLE} -s format icns ${_png} --out ${${_outvar}}
                DEPENDS ${_png}
                COMMENT "Generating ${_icns} with sips"
                VERBATIM)
            set_source_files_properties(${${_outvar}} PROPERTIES GENERATED TRUE)
        endif()
    endif()
    if(${_outvar})
        set_source_files_properties(${${_outvar}} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set(MACOSX_BUNDLE_ICON_FILE ${_icns})
    endif()
endmacro()

function(kate_set_bundle_properties _target _name _identifier _info)
    set_target_properties(${_target} PROPERTIES
        OUTPUT_NAME "${_name}"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in
        MACOSX_BUNDLE_BUNDLE_NAME "${_name}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${_identifier}"
        MACOSX_BUNDLE_INFO_STRING "${_info}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${KATE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${KATE_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${KATE_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "(c) 2000-2013 The Kate Authors"
    )
endfunction()
