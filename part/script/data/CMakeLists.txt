# JavaScript for indentation, command line commands, libraries and the
# "files" area where scripts keep additional data. The directory layout is
# preserved below katepart/script/ in the resource tree.
file(GLOB_RECURSE _kate_script_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     libraries/*.js commands/*.js indentation/*.js files/*)

set(_kate_script_dirs)
foreach(_f ${_kate_script_files})
    get_filename_component(_d ${_f} PATH)
    list(APPEND _kate_script_dirs ${_d})
endforeach()
list(REMOVE_DUPLICATES _kate_script_dirs)

foreach(_d ${_kate_script_dirs})
    set(_files)
    foreach(_f ${_kate_script_files})
        get_filename_component(_fd ${_f} PATH)
        if(_fd STREQUAL _d)
            list(APPEND _files ${CMAKE_CURRENT_SOURCE_DIR}/${_f})
        endif()
    endforeach()
    kate_install_data(katepart/script/${_d} FILES ${_files})
endforeach()
