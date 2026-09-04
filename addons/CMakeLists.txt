# Kate addons ported to the pure Qt 4 build. Each is a static plugin linked
# into the kate executable (kate_add_static_plugin, see
# cmake/KateQt4Macros.cmake); addons/kate/CMakeLists.txt lists the ported ones.
# The KTextEditor plugins (addons/ktexteditor) wait for the part's plugin
# loading (ROADMAP-QT4.md, task 4.6).
add_subdirectory(kate)
