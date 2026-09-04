# Kate on plain Qt 4: implementation roadmap

Status of the port and the plan for what comes next. Read with
BUILDING-QT4.md (how it builds) and qt4compat/README.md (the kdelibs
replacement layer). Dates are when a phase landed on branch `qt4-pure`.

The yardstick is BBEdit: a fast, robust everyday editor with first-rate
multi-file search, projects, a function scanner, git integration and
user-extensible language support. Reasonable C/C++ dependencies are fine
(pcre2, hunspell, libgit2, qjson); nothing monstrous. Prefer vendoring
kdelibs 4.14 code over rewriting it, but a small fresh implementation beats
dragging in a subsystem (KIO, kfile) for one widget.

## Work tiers

* **F (design)** - architecture, anything touching KatePart internals, the
  qt4compat kernel, the build system, regex/highlighting engines, plugin
  mechanisms, Qt 5 -> Qt 4 design decisions, reviews, diagnosing Mac build
  failures. Done directly in the main session.
* **S (routine)** - work that follows a written spec: porting an addon's
  CMakeLists to `kate_add_static_plugin()` + `K_IMPORT_PLUGIN()` + data
  install, vendoring a self-contained kdelibs widget given its source URL and
  the shim conventions, mechanical Qt 5 -> Qt 4 conversion of upstream code
  (lambdas to slots, `auto` to types, `QStringLiteral` to `QLatin1String`,
  `QRegularExpression` to `KateRegExp`), removing dead UI, running the syntax
  check and fixing includes, docs/Portfile/distfile sync, validation scripts.
  Delegated to a Sonnet agent with the spec, verified by the F tier.

Rules for both tiers: C++98/gnu++11, Qt 4 idioms only, every source passes
`qt4compat/tools/syntax-check.sh` on Linux before it goes to the Mac, no
cmake/build/link on the Linux box, one commit per logical change, port
artifacts (patches, overlay, Portfile checksums) regenerated with
`packaging/macports/make-port.sh` after each commit. Packaging rule: edits
to upstream files stay small hunks (they become patches); anything
substantial is a new file (it goes into the overlay); a file rewritten from
scratch is listed in `packaging/macports/replaced-files.txt`; never rely on
deleting an upstream file, the port does not delete.

## Done

* **Phase 0/1 (2026-09-02..04):** qt4compat layer, KatePart as one static
  library, KTextEditor interfaces, KWrite. Verified on macOS 10.15.
* **Phase 2 (2026-09-04):** the Kate application (kateinterfaces + kate,
  static plugins via `kate_add_static_plugin`/`K_IMPORT_PLUGIN`, KService
  from .desktop files, D-Bus and KIO jobs removed, per-bundle data). Verified
  on macOS 10.15.

## Phase 3: engine and languages (in progress)

| # | Task | Tier | Status |
|---|------|------|--------|
| 3.1 | `KatePcre` wrapper over pcre2 16-bit (UTF+UCP, LF newlines, JIT with interpreter fallback, per-copy match data so threads can share compiled code); `cmake/FindPCRE2.cmake`; Portfile dependency | F | done |
| 3.2 | Highlighting `RegExpr` rules on `KatePcre` (upstream semantics: `^` only at column 0, `minimal` = ungreedy, captures for dynamic rules, invalid patterns logged once and never match) | F | done |
| 3.3 | Document regex search (`KateRegExp`, used by the search bar, vi mode, `KTextEditor::SearchInterface`, the search plugin) on `KatePcre`; multi-line patterns get PCRE multiline `^`/`$` as in KF5 | F | done |
| 3.4 | Syntax loader: `<include>` in keyword lists (also `list##Language`), per-rule `weakDeliminator`/`additionalDeliminator` on keyword/Int/Float/HlCHex/HlCOct/WordDetect (153 upstream files use them) | F | done |
| 3.5 | Import the current upstream syntax-highlighting definitions (412 files; the PHP/Twig/Jinja/DoxygenLua/XSLT/JSONC variants are derived at build time with upstream's generators in part/syntax/generators, Perl + Python 3), drop the 4.14 set, add the upstream indenter scripts 4.14 lacks (julia, r) if their API use is compatible | S | done (r indenter added; julia needs ES6, left out) |
| 3.6 | Validation script for definitions: XML well-formedness, every `RegExpr` compiles in pcre2 16-bit mode (`~/barracuda/deproot/usr/bin/pcre2test -16` on Linux), cross-file `IncludeRules`/`<include>` targets exist, unique names, indenter names exist; run before each definitions update | S | done |
| 3.7 | Remove the dead "Download Highlighting Files" UI (its update server no longer exists; the set ships complete) | S | done |
| 3.8 | Multi-file search plugin (`addons/kate/search`): static plugin; folder/open-files/project scopes; regex via `KateRegExp` (PCRE like BBEdit's grep); shim: vendor `KAcceleratorManager` (kdelibs kdeui/util), write a local-only `KUrlCompletion` on `KCompletion`; verify thread use (`SearchDiskFiles`, `FolderFilesList` are QThreads: one `KatePcre` copy per thread) | S port, F review | done (also local KUrlRequester, KHistoryComboBox, KFile::Mode; kdelibs isBinaryData(fileName) overload) |
| 3.9 | Search plugin second pass: bring the KF5 21.12 improvements (results with highlighted preview, replace preview, search-as-you-type, per-tab searches) | F design, S port | later |
| 3.10 | BBEdit Codeless Language Module loader: scan `*.plist` in the syntax directories and BBEdit's own `Language Modules` folder, XML plist via QXmlStreamReader, map runs/keywords/patterns/comments/strings/suffixes into an in-memory Kate definition (per-run colors kept), section "BBEdit Modules"; function patterns feed the symbol viewer later | F | pending |

## Phase 4: everyday-editor addons

All are 4.14 sources unless noted; each becomes a static plugin with its
.desktop under `services/` and its `ui.rc` under `kate/plugins/<name>/`.

| # | Task | Tier | Notes |
|---|------|------|-------|
| 4.1 | symbolviewer (BBEdit function scanner), kate-ctags, tabbarextension, textfilter, openheader, backtracebrowser, xmlcheck, snippets tool view, exporter, hlselection, insertfile, kte_iconinserter | S | need nothing from the shim (hlselection/insertfile: trivial `KJob` forward header) |
| 4.2 | katebuild-plugin, close-except-like (vendor `KPassivePopup`), tabify, xmltools (vendor `KHistoryComboBox`), autobrace (small `KCModule` base) | S | |
| 4.3 | project plugin: qjson (MacPorts) for the .kateproject files, vendor `KRecursiveFilterProxyModel`, compile out the terminal tab; take Katana's libgit2 file listing (commit 9e8f013b) | S port, F libgit2 | BBEdit "Projects" |
| 4.4 | JavaScript editing commands for BBEdit's Text menu gaps: zap gremlins, educate/straighten quotes, prefix/suffix lines, add/remove line numbers, entab/detab; exposed in Tools > Scripts | S | |
| 4.5 | Cherry-pick Katana fixes: 16b94a99 session save with empty name, 9d52fb59 filebrowser sync, 3dd23f45 MIME check for highlighting | S | |
| 4.6 | KTextEditor plugin loading in the part (`katepartpluginmanager.cpp`) through the same static-plugin path as Kate plugins | F | needed by 4.1's ktexteditor plugins |

## Phase 5: spell checking

| # | Task | Tier |
|---|------|------|
| 5.1 | Vendor Sonnet core from kdelibs 4.14 (BackgroundChecker, Filter, Settings, Dialog, ConfigWidget, DictionaryComboBox) into qt4compat/src/sonnet, linking the hunspell client directly instead of loading it as a service; dictionary search paths for MacPorts and ~/Library/Spelling | F design, S vendoring |
| 5.2 | NSSpellChecker client as an optional second backend (system dictionaries) | later |

## Phase 6: git

| # | Task | Tier |
|---|------|------|
| 6.1 | Compare to HEAD: `git diff HEAD -- <file>` shown as a read-only Diff-highlighted document (reuse the swap-file diff pattern) | F |
| 6.2 | Git tool view: port upstream v21.12.3 `addons/project/gitwidget.cpp` + status model + commit/branches/stash dialogs (3.4k lines, git CLI through QProcess) onto Kate::MainWindow and Qt 4 | F design, S conversion |
| 6.3 | Blame as a tool view synced to the cursor line (upstream's inline-note plugin needs an API 4.14 lacks) | later |

## Phase 7: disk browser and the rest

| # | Task | Tier |
|---|------|------|
| 7.1 | Disk browser plugin on QFileSystemModel (BBEdit "disk browser"); the 4.14 filebrowser needs the kfile stack (KDirOperator, KUrlNavigator, KFilePlacesModel), not worth vendoring | F |
| 7.2 | Tab switcher (upstream tabswitcher, 0.9k lines), external tools with output pane (upstream externaltools, 2.3k lines), gdbplugin (needs KUrlRequester, KHistoryComboBox, KRandom) | S port |
| 7.3 | Find Differences between two open documents (diff CLI into a Diff-highlighted document) | F |
| 7.4 | In-document search bar and vi search consistency review on PCRE; clipboard history | later |

## Not planned

konsole and mailfiles (not needed for a text editor; revisit if ever),
katesql, replicode, pate, kttsd, lumen, LSP (Qt 5 JSON and threads, no
language servers for PowerPC), KIO remote files, zip browsing, Emacs
keybindings, translations (English only for now).
