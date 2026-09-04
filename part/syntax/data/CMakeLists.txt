# Syntax highlighting definitions.
#
# This is the current upstream set from KDE's syntax-highlighting framework
# (data/syntax/*.xml of https://invent.kde.org/frameworks/syntax-highlighting,
# snapshot 3e25191 of 2026-09-04), plus the definitions upstream generates at
# build time (the PHP, Twig and Jinja variants, DoxygenLua, XSLT, JSONC),
# generated once with upstream's data/generators scripts and committed here so
# the build needs neither Perl nor Python. KatePart reads them with the same
# rules and the same regex engine (pcre2) as upstream; see
# qt4compat/tools/validate-syntax.py for the checks to run on a new snapshot.
#
# language.dtd is the 4.14 DTD kept for reference, language.xsd the upstream
# schema.

file(GLOB HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

# The MacPorts port builds a patched upstream tree plus an overlay with the
# files above; nothing deletes files there, so the 4.14 definitions that
# upstream has since dropped or split (Fortran into fixed/free form) are
# still present in that tree. Keep them out so the modes do not appear twice.
set(SUPERSEDED_4_14_DEFINITIONS alert_indent.xml asterisk.xml chicken.xml fortran.xml)
foreach(_f ${SUPERSEDED_4_14_DEFINITIONS})
    list(REMOVE_ITEM HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

kate_install_data(katepart/syntax FILES language.dtd language.xsd syntax.template ${HIGHLIGHTERS})
