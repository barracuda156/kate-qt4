# Syntax highlighting definitions.
#
# This is the current upstream set from KDE's syntax-highlighting framework
# (data/syntax/*.xml of https://invent.kde.org/frameworks/syntax-highlighting,
# snapshot 3e25191 of 2026-09-04). The definitions upstream generates at build
# time - the PHP, Twig and Jinja variants of the web and config languages,
# DoxygenLua, XSLT, JSONC - are generated here the same way with upstream's
# scripts from ../generators (Perl and Python 3, standard modules only) into
# ${CMAKE_CURRENT_BINARY_DIR}/generated. KatePart reads them all with the same
# rules and the same regex engine (pcre2) as upstream. To check a new snapshot
# without a build, ../generators/generate-all.sh produces the variants and
# qt4compat/tools/validate-syntax.py validates both directories.
#
# language.dtd is the 4.14 DTD kept for reference, language.xsd the upstream
# schema.

set(GENERATORS ${CMAKE_CURRENT_SOURCE_DIR}/../generators)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})
set(GENERATED_HIGHLIGHTERS)

# <generator> <source>.xml <target>.xml; generate-html.pl takes the language
# merged into the source from the target's name (html-php.xml: php);
# extra arguments are dependencies only
macro(kate_generate_definition _generator _target _source)
    add_custom_command(OUTPUT ${GENERATED_DIR}/${_target}
        COMMAND ${PERL_EXECUTABLE} ${GENERATORS}/${_generator}
                ${CMAKE_CURRENT_SOURCE_DIR}/${_source} ${GENERATED_DIR}/${_target}
        DEPENDS ${GENERATORS}/${_generator} ${CMAKE_CURRENT_SOURCE_DIR}/${_source} ${ARGN}
        COMMENT "Generating ${_target}"
        VERBATIM)
    list(APPEND GENERATED_HIGHLIGHTERS ${GENERATED_DIR}/${_target})
endmacro()

foreach(_lang html css javascript javascript-react typescript mustache)
    kate_generate_definition(generate-html.pl ${_lang}-php.xml ${_lang}.xml ${CMAKE_CURRENT_SOURCE_DIR}/php.xml)
endforeach()
foreach(_lang html css javascript javascript-react typescript)
    kate_generate_definition(generate-html.pl ${_lang}-twig.xml ${_lang}.xml ${CMAKE_CURRENT_SOURCE_DIR}/twig.xml)
endforeach()
kate_generate_definition(generate-doxygenlua.pl doxygenlua.xml doxygen.xml)
kate_generate_definition(generate-xslt.pl xslt.xml xml.xml ${GENERATORS}/xslt.base.xml)
kate_generate_definition(generate-jsonc.pl jsonc.xml json.xml)

# Jinja variants: the script also produces variants of the definitions the
# listed ones include, so ask it for the output list first (--dry-run)
set(JINJA_LANGS html css xml json yaml toml sql python cpp qml java groovy rust
    javascript typescript ruby cmake qmake ini bash zsh dockerfile terraform systemd-unit)
set(JINJA_INPUTS)
foreach(_lang ${JINJA_LANGS})
    list(APPEND JINJA_INPUTS ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.xml)
endforeach()
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${GENERATORS}/generate_jinja.py --dry-run
            --jinja-xml ${CMAKE_CURRENT_SOURCE_DIR}/jinja.xml --prefix jinja-
            --output-dir ${GENERATED_DIR} ${JINJA_INPUTS}
    OUTPUT_VARIABLE _jinja_outputs
    RESULT_VARIABLE _jinja_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT _jinja_result EQUAL 0 OR NOT _jinja_outputs)
    message(FATAL_ERROR "generate_jinja.py --dry-run failed (${_jinja_result}); ${PYTHON_EXECUTABLE} must be a Python 3")
endif()
string(REPLACE "\r" "" _jinja_outputs "${_jinja_outputs}")
string(REPLACE "\n" ";" _jinja_outputs "${_jinja_outputs}")
add_custom_command(OUTPUT ${_jinja_outputs}
    COMMAND ${PYTHON_EXECUTABLE} ${GENERATORS}/generate_jinja.py
            --jinja-xml ${CMAKE_CURRENT_SOURCE_DIR}/jinja.xml --prefix jinja-
            --output-dir ${GENERATED_DIR} ${JINJA_INPUTS}
    DEPENDS ${GENERATORS}/generate_jinja.py ${CMAKE_CURRENT_SOURCE_DIR}/jinja.xml ${JINJA_INPUTS}
    COMMENT "Generating the Jinja template variants"
    VERBATIM)
list(APPEND GENERATED_HIGHLIGHTERS ${_jinja_outputs})

file(GLOB HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

# The MacPorts port builds a patched upstream tree plus an overlay with the
# files above; nothing deletes files there, so the 4.14 definitions that
# upstream has since dropped or split (Fortran into fixed/free form) or
# now derives at build time (DoxygenLua, XSLT: generated above) are still
# present in that tree. Keep them out so the modes do not appear twice.
# packaging/macports/unused-upstream-files.txt lists them for make-port.sh.
set(SUPERSEDED_4_14_DEFINITIONS alert_indent.xml asterisk.xml chicken.xml fortran.xml doxygenlua.xml xslt.xml)
foreach(_f ${SUPERSEDED_4_14_DEFINITIONS})
    list(REMOVE_ITEM HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

kate_install_data(katepart/syntax FILES language.dtd language.xsd syntax.template ${HIGHLIGHTERS} ${GENERATED_HIGHLIGHTERS})
