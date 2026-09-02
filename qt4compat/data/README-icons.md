# Bundled icon theme

`data/icons/oxygen/` is a subset of the KDE Oxygen icon theme, taken from
https://invent.kde.org/frameworks/oxygen-icons.git at commit
79ae8a5bd1a35a0219b42e021963cd0a0cc76865 (2026-08-28). Only the icon names
used by Kate, KWrite, KatePart and the qt4compat widgets are included, at
16x16, 22x22, 32x32 and 48x48 pixels, plus 128x128 for the application icons
(`accessories-text-editor`, `kate`). The theme is LGPL-3 (see
`data/icons/oxygen/COPYING`).

The files are compiled in through `data/icons.qrc` under the `:/icons`
prefix; `KIconLoader::global()` registers that path with
`QIcon::setThemeSearchPaths()` and selects the theme, so plain
`QIcon::fromTheme("document-open")` works everywhere.

To add an icon: copy the PNGs into the matching `<size>x<size>/<context>/`
directories, add them to `icons.qrc`, and make sure the directory is listed
in `index.theme`.
