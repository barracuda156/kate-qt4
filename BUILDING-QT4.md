# Building Kate on plain Qt 4 (no kdelibs)

Branch `qt4-pure` builds KatePart, KWrite and the Kate application against
Qt 4.8 only. The
kdelibs classes the sources need come from the `qt4compat/` layer (see
`qt4compat/README.md`). Nothing else is required: no KDE runtime, no D-Bus,
no kdeinit, no X11.

## Requirements

* CMake 3.5 or newer (`FindQt4.cmake` is vendored under `cmake/`, so CMake 4
  works too).
* Qt 4.8.x with QtCore, QtGui, QtXml, QtScript, QtNetwork (QtScript drives
  the indenters and editor commands).
* zlib; bzip2 optional (bzip2-compressed documents).
* pcre2 (the 16-bit library, `libpcre2-16`): the regular expression engine
  of syntax highlighting and search, the same engine upstream Kate uses
  through QRegularExpression, so the shipped highlighting definitions behave
  identically. JIT is used when the library provides it.
* A C++ compiler; the sources are C++98 and build with `-std=gnu++11` (the
  default set by CMake >= 3.1) which also avoids `register`-keyword errors
  with new GCCs.

## macOS

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_PREFIX_PATH=/opt/local/libexec/qt4 \
             -DQT_QMAKE_EXECUTABLE=/opt/local/libexec/qt4/bin/qmake \
             -DKATE_BUILD_KWRITE=ON \
             -DKATE_BUILD_KATE=ON \
             -DKATE_INSTALL_BUNDLE_DIR=/Applications
    make
    make install

`Kate.app` and `KWrite.app` are self-contained bundles; each gets its own
copy of the data files it needs (XMLGUI `.rc`, syntax XML, indenter and
command scripts, default configs, plugin `.desktop` files) in
`Contents/Resources`, found through `KStandardDirs`. User settings go to
`~/Library/Preferences/Kate`, user data (custom syntax files, sessions) to
`~/Library/Application Support/Kate`.

## Linux (smoke tests only)

    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local
    make

Data is installed to `<prefix>/share/kate-qt4`, the binaries to `<prefix>/bin`.

## Running from the build directory

The build stages every data file into `<build>/share` (target `kate-data`),
and that directory is compiled in as a fallback search path, so `kwrite`
and `kate` run uninstalled. To point a binary at another data tree set
`KATE_RESOURCE_DIRS=/path/one:/path/two` (searched first). `KATE_DEBUG=1`
enables the `kDebug()` output.

## The Kate application

`kate/` builds into the static library `kateinterfaces` (the `Kate::*`
plugin interfaces and the application classes) and the `kate` executable.
Plugins are not loaded at run time: a plugin is a static library made with
`kate_add_static_plugin()`, which registers it in the global CMake property
`KATE_STATIC_PLUGINS`; `kate/app` links every registered plugin and
generates `katebuiltinplugins.cpp` (from `katebuiltinplugins.cpp.in`) with
one `K_IMPORT_PLUGIN()` per plugin. A plugin's `.desktop` file, installed
under `services/` in the data tree, is what
`KServiceTypeTrader::query("Kate/Plugin")` lists. The document tree
(`kate/filetree/`) and the addons under `addons/kate/` (option
`KATE_BUILD_ADDONS`, on; `addons/kate/CMakeLists.txt` lists the ported
ones, starting with the multi-file search plugin) are built this way.
Every `kate` invocation starts a new instance (the D-Bus "reuse the running
Kate" logic is gone); on macOS Finder open requests reach the running
instance through `QFileOpenEvent`. Dropping a folder onto Kate opens the
files below it (a local directory walk instead of a `KIO::listRecursive`
job).

## Syntax highlighting definitions

`part/syntax/data/` carries the current upstream set from KDE's
syntax-highlighting framework (the snapshot is named in its
`CMakeLists.txt`), including the variants upstream generates at build time,
generated once and committed. KatePart reads them with upstream's rules:
per-rule word delimiters, `<include>` in keyword lists, pcre2 regexes.
`qt4compat/tools/validate-syntax.py` checks a new snapshot (well-formed XML,
every regex compiles in pcre2 16-bit mode, cross references resolve) and
needs `pcre2test`; on the Linux box it lives in `~/barracuda/deproot`.
User definitions go into `~/Library/Application Support/Kate/katepart/syntax/`
(macOS) or `$XDG_DATA_HOME/kate-qt4/katepart/syntax/`.

## What is not built yet

* most addons (see ROADMAP-QT4.md for the order; `addons/kate/CMakeLists.txt`
  has the ones that are);
* tests (`tests/`, option `BUILD_TESTING`, off);
* spell checking (Sonnet is stubbed), remote files (no KIO), "Get Hot New
  Stuff" downloads, D-Bus scripting, X11 session management, translations.
