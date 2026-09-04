# cmake/MakeIcns.cmake - fallback used by kate_macos_app_icon() when no
# prebuilt qt4compat/data/<name>.icns exists: run sips and fail loudly if it
# cannot write the file (sips writes icns from 10.7 on; before that it exits
# 0 with "Unsupported output format com.apple.icns" and no output).
#   cmake -DSIPS=<sips> -DPNG=<in.png> -DICNS=<out.icns> -P MakeIcns.cmake
execute_process(
    COMMAND ${SIPS} -s format icns ${PNG} --out ${ICNS}
    RESULT_VARIABLE _result
    OUTPUT_VARIABLE _output
    ERROR_VARIABLE _error)
if(NOT _result EQUAL 0 OR NOT EXISTS ${ICNS})
    file(REMOVE ${ICNS})
    message(FATAL_ERROR "sips could not write ${ICNS}: ${_output}${_error}"
        "This macOS cannot write icns with sips; add a prebuilt icon "
        "(qt4compat/data/, made with qt4compat/tools/make-icns.py) instead.")
endif()
