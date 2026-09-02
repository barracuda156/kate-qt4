########### syntax highlighting definitions ###############

# the php variants of css/html/javascript are generated with perl
if(NOT PERL_EXECUTABLE)
    find_package(Perl)
endif()

set(GENERATED_PHP_XML)
if(PERL_EXECUTABLE)
    macro(GENERATE_PHP_XML targetFile srcFile)
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
            COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate-php.pl < ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile} > ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile} ${CMAKE_CURRENT_SOURCE_DIR}/generate-php.pl)
        list(APPEND GENERATED_PHP_XML ${CMAKE_CURRENT_BINARY_DIR}/${targetFile})
    endmacro(GENERATE_PHP_XML)

    generate_php_xml(javascript-php.xml javascript.xml)
    generate_php_xml(css-php.xml css.xml)
    generate_php_xml(html-php.xml html.xml)

    add_custom_target(GeneratePhpXmlFiles ALL DEPENDS ${GENERATED_PHP_XML})
else()
    message(WARNING "perl not found: the css-php/html-php/javascript-php highlighting files will not be generated")
endif()

file(GLOB HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

kate_install_data(katepart/syntax FILES language.dtd syntax.template ${HIGHLIGHTERS} ${GENERATED_PHP_XML})
