# Kate MDI application on plain Qt 4: the Kate::* plugin interfaces and the
# application core (app/, static library kateinterfaces + the kate
# executable), the built-in plugins (filetree/) and the data files (data/).
# Plugins are linked into the executable; nothing is loaded at run time.

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=13001)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/interfaces)

add_subdirectory(app)
add_subdirectory(filetree)
add_subdirectory(data)
