# Kate application data: the XMLGUI definition and the default settings.
# Not installed: kateplugin.desktop (a ksycoca service type definition; the
# trader here matches on the ServiceTypes= line of the plugin files
# directly), kate-2.4.upd (kconf_update), tips (KTipDialog is a stub).

kate_install_data(kate FILES kateui.rc APPS kate)
kate_install_data(config FILES katerc APPS kate)

if(NOT APPLE)
    install(PROGRAMS kate.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()

# kate application icons
add_subdirectory(icons)
