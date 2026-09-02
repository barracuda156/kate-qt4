#!/bin/sh
# qt4compat/tools/syntax-check.sh - compile-only check against Qt 4.8 headers.
#
#   syntax-check.sh [-I dir]... [-D def]... [-std=...] file.cpp [file2.cpp ...]
#
# Environment: QT4ROOT (default /home/sgr/barracuda/qt4root/usr),
#              SYNTAX_GEN_DIR (default: scratch dir under /tmp).
# Runs moc/uic for `#include "x.moc"` and `#include "ui_x.h"` found in the
# sources, then g++ -fsyntax-only. Never links or produces binaries.

set -u
QT4ROOT="${QT4ROOT:-/home/sgr/barracuda/qt4root/usr}"
QTINC="$QT4ROOT/include/qt4"
MOC="$QT4ROOT/lib/x86_64-linux-gnu/qt4/bin/moc"
UIC="$QT4ROOT/lib/x86_64-linux-gnu/qt4/bin/uic"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
GEN="${SYNTAX_GEN_DIR:-/tmp/claude-1001/-home-sgr-barracuda-kate/30204108-34cd-4ff0-a116-09cdaddc4988/scratchpad/syntax-gen}"
mkdir -p "$GEN"

STD="-std=gnu++11"
EXTRA=""
FILES=""
for a in "$@"; do
    case "$a" in
        -std=*) STD="$a" ;;
        -I*|-D*|-U*|-W*|-f*) EXTRA="$EXTRA $a" ;;
        *) FILES="$FILES $a" ;;
    esac
done

# minimal config.h as CMake would generate it (Linux values)
if [ ! -f "$GEN/config.h" ]; then
    cat > "$GEN/config.h" <<'EOC'
#ifndef KATE_CONFIG_H
#define KATE_CONFIG_H
#define HAVE_FDATASYNC 1
#define HAVE_CTERMID 1
#define KATE_VERSION "4.14.3"
#endif
EOC
fi

INCS="-I$GEN -I$ROOT/qt4compat/include -I$ROOT -I$ROOT/includes -I$ROOT/ktexteditor"
INCS="$INCS -I$QTINC -I$QTINC/QtCore -I$QTINC/QtGui -I$QTINC/QtXml -I$QTINC/QtScript -I$QTINC/QtNetwork -I$QTINC/Qt"
DEFS="-DQT_NO_KEYWORDS_NOT_USED -D_LARGEFILE64_SOURCE -DKATE_INSTALL_DATADIR=\"/usr/share/kate-qt4\""

fail=0
total=0
for f in $FILES; do
    total=$((total + 1))
    dir="$(cd "$(dirname "$f")" && pwd)"
    base="$(basename "$f")"
    rel="${dir#$ROOT/}"
    gdir="$GEN/$rel"
    mkdir -p "$gdir"
    # moc for "#include "x.moc""
    for m in $(grep -oE '#include\s+["<][A-Za-z0-9_]+\.moc[">]' "$f" | sed -E 's/.*["<]([^">]+)\.moc[">]/\1/'); do
        src=""
        if [ -f "$dir/$m.h" ] && grep -q 'Q_OBJECT\|Q_GADGET' "$dir/$m.h"; then src="$dir/$m.h"
        elif [ -f "$dir/$m.cpp" ] && grep -q 'Q_OBJECT\|Q_GADGET' "$dir/$m.cpp"; then src="$dir/$m.cpp"
        elif [ -f "$dir/$m.h" ]; then src="$dir/$m.h"
        else
            # header lives elsewhere (qt4compat keeps headers in include/): search the include path
            for idir in "$ROOT/qt4compat/include" "$ROOT/qt4compat/include/kparts" "$ROOT/qt4compat/include/sonnet" $(echo "$EXTRA" | tr ' ' '\n' | sed -n 's/^-I//p'); do
                if [ -f "$idir/$m.h" ]; then src="$idir/$m.h"; break; fi
            done
            # private headers next to the source: X_p.h is not the moc source; keep cpp fallback
            [ -z "$src" ] && [ -f "$dir/$m.cpp" ] && src="$dir/$m.cpp"
        fi
        if [ -n "$src" ]; then
            "$MOC" $INCS -I"$dir" $DEFS $EXTRA "$src" -o "$gdir/$m.moc" 2>"$gdir/$m.moc.err" || { echo "moc failed: $src"; cat "$gdir/$m.moc.err"; }
        else
            echo "warning: no source for $m.moc (included by $f)"
        fi
    done
    # uic for "#include "ui_x.h""
    for u in $(grep -oE '#include\s+"ui_[A-Za-z0-9_]+\.h"' "$f" | sed -E 's/.*"ui_([^"]+)\.h"/\1/'); do
        ui=""
        for cand in "$dir/$u.ui" "$dir"/*/"$u.ui" "$dir/../$u.ui"; do
            [ -f "$cand" ] && { ui="$cand"; break; }
        done
        if [ -n "$ui" ]; then
            "$UIC" "$ui" -o "$gdir/ui_$u.h" 2>"$gdir/ui_$u.err" || { echo "uic failed: $ui"; cat "$gdir/ui_$u.err"; }
        else
            echo "warning: no .ui for ui_$u.h (included by $f)"
        fi
    done
    if g++ $STD -fsyntax-only -Wall -Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-local-typedefs \
        $INCS -I"$gdir" -I"$dir" $DEFS $EXTRA "$f" 2>"$gdir/$base.err"; then
        rm -f "$gdir/$base.err"
    else
        fail=$((fail + 1))
        echo "=== FAIL: $f"
        grep -E 'error|Error' "$gdir/$base.err" | head -40
    fi
done
echo "syntax-check: $total files, $fail failed"
[ "$fail" -eq 0 ]
