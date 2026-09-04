# Application icon. On macOS the bundle's kate.icns is generated from the
# 128/32/16 pixel PNGs here by kate_macos_app_icon() in ../../app/CMakeLists.txt;
# the in-application icon ("kate" in KIcon lookups) comes from the icon
# theme compiled into qt4compat. Elsewhere install the hicolor set.
if(NOT APPLE)
    foreach(_size 16 22 32 48 64 128)
        install(FILES hi${_size}-app-kate.png
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/${_size}x${_size}/apps
                RENAME kate.png)
    endforeach()
    install(FILES hisc-app-kate.svgz
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
            RENAME kate.svgz)
endif()
