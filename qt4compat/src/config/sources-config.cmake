# qt4compat: vendored KConfig stack, KStringHandler and KColorScheme/KColorUtils (agent A)
list(APPEND QT4COMPAT_CONFIG_SRCS
    src/config/kconfig.cpp
    src/config/kconfigbase.cpp
    src/config/kconfigbackend.cpp
    src/config/kconfigdata.cpp
    src/config/kconfiggroup.cpp
    src/config/kconfigini.cpp
    src/config/ksharedconfig.cpp
)
list(APPEND QT4COMPAT_CORE_SRCS
    src/core/kstringhandler.cpp
)
list(APPEND QT4COMPAT_GUI_SRCS
    src/gui/kcolorscheme.cpp
    src/gui/kcolorutils.cpp
    src/gui/kcolorspaces.cpp
    src/gui/kcolorhelpers.cpp
)
list(APPEND QT4COMPAT_MOC_HEADERS
    include/kconfigbackend.h
)
