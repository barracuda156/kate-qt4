#!/bin/bash
# packaging/macports/make-port.sh - build the MacPorts port of this tree.
#
#   make-port.sh [-o <outdir>] [-r <revision>] [<commit>]
#
# The port is "upstream Kate at one commit + patches + one overlay tarball":
#   * base:    GitHub's archive of the upstream commit in upstream-base
#              (fetched by the Portfile through the github PortGroup);
#   * patches: files/patch-<area>.diff, one per top-level source area, holding
#              every change to an upstream file that is not a whole replacement;
#   * overlay: kate-qt4-overlay-<version>.tar.xz with every file that does
#              not exist upstream plus the replaced upstream files listed in
#              replaced-files.txt (packaging/ itself is left out: it is not
#              source). Unpacked over the extracted sources before patching.
#   Nothing is deleted in the port; files this build no longer uses stay in
#   the tree unused, and every such file must be listed with its reason in
#   unused-upstream-files.txt (the script refuses unknown ones).
#
# Writes <outdir>/kate-qt4/{Portfile,files/}, <outdir>/distfiles/<overlay>,
# <outdir>/upstream-changes.txt and <outdir>/overlay-contents.txt, updates the
# checksums / patchfiles / revision lines of the in-tree Portfile, and then
# reconstructs the tree from base + patches + overlay in a scratch directory
# and compares it with the commit (must be identical apart from the deleted
# files and packaging/). The base archive is downloaded once into
# <outdir>/distfiles/. Needs git, xz, patch, python3, curl.
#
# Default <outdir>: ~/barracuda/kate-qt4-port. Run it on a clean tree after
# committing; commit the updated Portfile afterwards ("sync" commit).

set -eu

OUT=${OUT:-$HOME/barracuda/kate-qt4-port}
REVISION=""
while getopts "o:r:" opt; do
    case $opt in
        o) OUT=$OPTARG ;;
        r) REVISION=$OPTARG ;;
        *) echo "usage: $0 [-o outdir] [-r revision] [commit]" >&2; exit 2 ;;
    esac
done
shift $((OPTIND - 1))
COMMIT=${1:-HEAD}

ROOT=$(git rev-parse --show-toplevel)
cd "$ROOT"
HERE=packaging/macports
BASE=$(cat $HERE/upstream-base)
PORTFILE=$HERE/kate-qt4/Portfile
VERSION=$(sed -n 's/^version[[:space:]]*//p' $PORTFILE)
[ -n "$VERSION" ] || { echo "no version in $PORTFILE" >&2; exit 1; }
COMMIT_SHA=$(git rev-parse "$COMMIT")

if [ "$COMMIT" = "HEAD" ] && [ -n "$(git status --porcelain)" ]; then
    echo "warning: the working tree is not clean; artifacts are made from the commit, not the tree" >&2
fi

PORTDIR=$OUT/kate-qt4
FILESDIR=$PORTDIR/files
DIST=$OUT/distfiles
mkdir -p "$FILESDIR" "$DIST"
TMP=$(mktemp -d "${TMPDIR:-/tmp}/make-port.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

# ---- classify the differences to the upstream base ------------------------
git diff --no-renames --diff-filter=M --name-only "$BASE" "$COMMIT_SHA" > "$TMP/modified"
git diff --no-renames --diff-filter=A --name-only "$BASE" "$COMMIT_SHA" > "$TMP/added"
git diff --no-renames --diff-filter=D --name-only "$BASE" "$COMMIT_SHA" > "$TMP/deleted"

replaced_patterns=$(grep -v '^[[:space:]]*#' $HERE/replaced-files.txt | grep -v '^[[:space:]]*$' || true)
is_replaced() {
    local f=$1 p
    for p in $replaced_patterns; do
        case "$f" in $p) return 0 ;; esac
    done
    return 1
}

: > "$TMP/patched"; : > "$TMP/replaced"
while read -r f; do
    if is_replaced "$f"; then echo "$f" >> "$TMP/replaced"; else echo "$f" >> "$TMP/patched"; fi
done < "$TMP/modified"

# every deleted upstream file must be known to be harmless when left behind
unlisted=$(while read -r f; do
    grep -q "^$f[[:space:]]" $HERE/unused-upstream-files.txt || echo "  $f"
done < "$TMP/deleted")
if [ -n "$unlisted" ]; then
    echo "deleted upstream files not listed in $HERE/unused-upstream-files.txt (the port does not delete; make sure they are harmless, then list them):" >&2
    echo "$unlisted" >&2
    exit 1
fi

# overlay = added files (not packaging/) + replaced files
{ grep -v '^packaging/' "$TMP/added"; cat "$TMP/replaced"; } | sort > "$TMP/overlay"

# a patched file that lost most of its lines is probably a rewrite: say so
while read -r f; do
    stat=$(git diff --no-renames --numstat "$BASE" "$COMMIT_SHA" -- "$f" | cut -f1,2)
    added=${stat%%	*}; removed=${stat##*	}
    total=$(git show "$BASE:$f" | wc -l | tr -d ' ')
    if [ "$total" -gt 0 ] && [ "$removed" -ge $((total * 7 / 10)) ]; then
        echo "note: $f removes $removed of $total lines; consider listing it in replaced-files.txt"
    fi
done < "$TMP/patched"

# ---- patches, one per area ------------------------------------------------
rm -f "$FILESDIR"/patch-*.diff
area_of() {
    case $1 in
        part/*) echo part ;;
        ktexteditor/*) echo ktexteditor ;;
        kwrite/*) echo kwrite ;;
        kate/*) echo kate ;;
        addons/*) echo addons ;;
        *) echo misc ;;
    esac
}
patchfiles=""
for area in part ktexteditor kwrite kate addons misc; do
    paths=$(while read -r f; do if [ "$(area_of "$f")" = "$area" ]; then echo "$f"; fi; done < "$TMP/patched")
    [ -n "$paths" ] || continue
    # shellcheck disable=SC2086
    git diff --no-renames "$BASE" "$COMMIT_SHA" -- $paths > "$FILESDIR/patch-$area.diff"
    if [ -s "$FILESDIR/patch-$area.diff" ]; then
        patchfiles="$patchfiles patch-$area.diff"
    else
        rm -f "$FILESDIR/patch-$area.diff"
    fi
done
patchfiles=${patchfiles# }

# ---- overlay tarball ------------------------------------------------------
# Reproducible: git archive would stamp the commit id and its date into the
# tar, so the same content would get a new checksum after every commit (the
# Portfile sync commit included). Export the files and tar them with fixed
# owner, sorted names and the date of the last commit that touched any of
# them (GNU tar; the script runs on the Linux box).
OVERLAY=kate-qt4-overlay-$VERSION.tar.xz
mkdir -p "$TMP/export"
git archive --format=tar "$COMMIT_SHA" $(cat "$TMP/overlay") | tar -x -C "$TMP/export"
OVERLAY_MTIME=$(git log -1 --format=%ct "$COMMIT_SHA" -- $(cat "$TMP/overlay"))
tar --format=ustar --sort=name --owner=0 --group=0 --numeric-owner --mtime="@$OVERLAY_MTIME" \
    -cf - -C "$TMP/export" $(cat "$TMP/overlay") | xz -9 > "$DIST/$OVERLAY"

# ---- base archive (for checksums and the check below) ---------------------
BASEFILE=$DIST/kate-$BASE.tar.gz
if [ ! -f "$BASEFILE" ]; then
    echo "downloading the upstream base archive"
    curl -sL -o "$BASEFILE" "https://codeload.github.com/KDE/kate/legacy.tar.gz/$BASE"
fi

sums() {
    printf 'rmd160  %s\nsha256  %s\nsize    %s\n' \
        "$(openssl dgst -ripemd160 -r "$1" | cut -d' ' -f1)" \
        "$(sha256sum "$1" | cut -d' ' -f1)" \
        "$(stat -c %s "$1" 2>/dev/null || stat -f %z "$1")"
}
sums "$BASEFILE" > "$TMP/base.sums"
sums "$DIST/$OVERLAY" > "$TMP/overlay.sums"

# ---- Portfile: checksums, patchfiles, revision ----------------------------
python3 - "$PORTFILE" "$TMP/base.sums" "$TMP/overlay.sums" "$patchfiles" "$REVISION" <<'PY'
import re, sys
path, base_sums, overlay_sums, patchfiles, revision = sys.argv[1:6]
s = open(path).read()
def block(sumsfile):
    return ''.join('                    %s \\\n' % l.strip() for l in open(sumsfile))
new = ('checksums           ${distname}${extract.suffix} \\\n' + block(base_sums)
       + '                    ${overlay} \\\n' + block(overlay_sums)).rstrip(' \\\n') + '\n'
# the block is the "checksums" line plus every line that follows a line
# ending in a backslash
lines = s.split('\n')
start = next(i for i, l in enumerate(lines) if l.startswith('checksums'))
end = start
while lines[end].rstrip().endswith('\\'):
    end += 1
lines[start:end + 1] = new.rstrip('\n').split('\n')
s = '\n'.join(lines)
s, n = re.subn(r'^patchfiles[ \t]+[^\n]*\n', 'patchfiles          %s\n' % patchfiles, s, count=1, flags=re.M)
assert n == 1, 'patchfiles line not found'
if revision:
    s, n = re.subn(r'^revision[ \t]+\d+', 'revision            %s' % revision, s, count=1, flags=re.M)
    assert n == 1
open(path, 'w').write(s)
PY
cp "$PORTFILE" "$PORTDIR/Portfile"
cp "$TMP/patched" "$OUT/upstream-changes.txt"
cp "$TMP/overlay" "$OUT/overlay-contents.txt"

# ---- reconstruct and compare ---------------------------------------------
mkdir -p "$TMP/port" "$TMP/git"
tar -xzf "$BASEFILE" -C "$TMP/port"
SRC=$(ls -d "$TMP"/port/*/)
( cd "$SRC" && xz -dc "$DIST/$OVERLAY" | tar -xf - )
for p in $patchfiles; do
    ( cd "$SRC" && patch -p1 -s < "$FILESDIR/$p" )
done
git archive --format=tar "$COMMIT_SHA" | tar -x -C "$TMP/git"
# expected differences: deleted upstream files stay in the port tree, packaging/ is not shipped
diff -rq "$SRC" "$TMP/git" > "$TMP/diff.txt" 2>&1 || true
grep -v '^Only in .*/port/[^:]*: ' "$TMP/diff.txt" | grep -v ': packaging$' > "$TMP/unexpected" || true
while read -r f; do
    grep -q "^Only in .*: $(basename "$f")\$" "$TMP/diff.txt" || echo "expected deleted file not found in the port tree: $f" >> "$TMP/unexpected"
done < "$TMP/deleted"
if [ -s "$TMP/unexpected" ]; then
    echo "RECONSTRUCTION DIFFERS:"; cat "$TMP/unexpected"; exit 1
fi

echo "base:     kate-$BASE.tar.gz ($(sed -n 's/^size *//p' "$TMP/base.sums") bytes)"
echo "overlay:  $OVERLAY ($(wc -l < "$TMP/overlay") files, $(sed -n 's/^size *//p' "$TMP/overlay.sums") bytes)"
echo "patches:  $patchfiles ($(wc -l < "$TMP/patched") upstream files changed, $(cat "$FILESDIR"/patch-*.diff | wc -c) bytes)"
echo "unused upstream files left in the port tree: $(wc -l < "$TMP/deleted")"
echo "reconstruction from base + overlay + patches matches $COMMIT_SHA"
echo "Portfile: $PORTDIR/Portfile (in-tree copy updated: $PORTFILE)"
