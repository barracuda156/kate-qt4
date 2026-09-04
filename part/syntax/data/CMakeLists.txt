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

kate_install_data(katepart/syntax FILES language.dtd language.xsd syntax.template ${HIGHLIGHTERS})
