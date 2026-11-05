# Kate addons ported to the pure Qt 4 build. Each is a static plugin
# (kate_add_static_plugin / kate_add_static_editor_plugin, see
# cmake/KateQt4Macros.cmake): the KatePart editor plugins in ktexteditor/ are
# linked into every application embedding the part (kwrite, kate), the Kate
# application plugins in kate/ into kate only. The CMakeLists in each
# directory lists the ported ones.
add_subdirectory(ktexteditor)
if(KATE_BUILD_KATE)
    add_subdirectory(kate)
endif()
