# KateUic.cmake - KDE 4 flavoured uic (what kdelibs' kde4uic.cmake did):
# the strings go through tr2i18n() and the generated header starts with
# kdialog.h and klocale.h, which upstream sources rely on transitively.
#
#   cmake -DUIC=<uic> -DINPUT=<file.ui> -DOUTPUT=<ui_file.h> -P KateUic.cmake
execute_process(COMMAND ${UIC} -tr tr2i18n ${INPUT}
                OUTPUT_VARIABLE _contents RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
    message(FATAL_ERROR "uic failed on ${INPUT}")
endif()
string(REGEX REPLACE "tr2i18n\\(\"\", 0\\)" "QString()" _contents "${_contents}")
string(REGEX REPLACE "tr2i18n\\(\"\"\\)" "QString()" _contents "${_contents}")
file(WRITE ${OUTPUT} "#include <kdialog.h>\n#include <klocale.h>\n\n${_contents}\n")
