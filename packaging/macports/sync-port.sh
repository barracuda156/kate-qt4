#!/bin/bash
# packaging/macports/sync-port.sh - point the Portfile at a pushed tag or
# commit of the hosted repository and write its checksums.
#
#   sync-port.sh [-o <outdir>] [-r <revision>] [-v <version>] <tag-or-commit>
#
# The port fetches GitHub's archive of a tag (github.setup ... <version> v,
# tag v<version>) or of a commit hash of https://github.com/<author>/<repo>.
# This downloads that archive, so the ref must already be pushed; checks that
# its content equals the local commit it names; writes the github.setup,
# version, revision and checksums lines of the in-tree Portfile and copies it
# to <outdir>/kate-qt4/Portfile (default <outdir>: ~/barracuda/kate-qt4-port,
# the archive lands in <outdir>/distfiles/ under the name the port uses). For
# a commit the version is <latest tag version>.<commit date YYYYMMDD> unless
# -v says otherwise. Commit the updated Portfile afterwards ("sync" commit).
# Needs git, curl, tar, openssl, python3.
set -eu
AUTHOR=barracuda156
REPO=kate-qt4
REMOTE=origin
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
PORTFILE="$HERE/kate-qt4/Portfile"
OUT="$HOME/barracuda/kate-qt4-port"
REVISION=0
VERSION=""
while getopts "o:r:v:" opt; do
    case $opt in
        o) OUT=$OPTARG ;;
        r) REVISION=$OPTARG ;;
        v) VERSION=$OPTARG ;;
        *) exit 2 ;;
    esac
done
shift $((OPTIND - 1))
[ $# -eq 1 ] || { sed -n '2,17p' "$0"; exit 2; }
REF=$1

cd "$ROOT"
git fetch -q "$REMOTE" --tags
case "$REF" in
    v[0-9]*)
        TAG=$REF
        GH_VERSION=${REF#v}
        SETUP="github.setup        $AUTHOR $REPO $GH_VERSION v"
        [ -n "$VERSION" ] || VERSION=$GH_VERSION
        SHA=$(git rev-parse "refs/tags/$TAG^{commit}")
        URL="https://github.com/$AUTHOR/$REPO/archive/refs/tags/$TAG.tar.gz"
        ;;
    *)
        SHA=$(git rev-parse --verify "$REF^{commit}")
        GH_VERSION=$SHA
        SETUP="github.setup        $AUTHOR $REPO $SHA"
        if [ -z "$VERSION" ]; then
            last=$(git describe --tags --abbrev=0 --match 'v*' "$SHA" 2>/dev/null || echo v4.14.3.qt4.0)
            VERSION="${last#v}.$(git log -1 --format=%cd --date=format:%Y%m%d "$SHA")"
        fi
        URL="https://github.com/$AUTHOR/$REPO/archive/$SHA.tar.gz"
        ;;
esac
git merge-base --is-ancestor "$SHA" "$REMOTE/qt4-pure" \
    || { echo "$SHA is not on $REMOTE/qt4-pure: push first"; exit 1; }

DIST="$OUT/distfiles"
mkdir -p "$DIST" "$OUT/kate-qt4"
DISTFILE="$DIST/$REPO-$GH_VERSION.tar.gz"
echo "downloading $URL"
curl -fsSL -o "$DISTFILE" "$URL"

# the archive must be the tree of the commit (GitHub honours export-ignore
# like git archive does)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/sync-port.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
mkdir "$TMP/github" "$TMP/git"
tar -xzf "$DISTFILE" -C "$TMP/github" --strip-components=1
git archive --format=tar "$SHA" | tar -x -C "$TMP/git"
diff -r "$TMP/github" "$TMP/git" > "$TMP/diff" || { echo "archive differs from commit $SHA:"; cat "$TMP/diff"; exit 1; }

RMD160=$(openssl dgst -ripemd160 -r "$DISTFILE" | cut -d' ' -f1)
SHA256=$(sha256sum "$DISTFILE" | cut -d' ' -f1)
SIZE=$(stat -c %s "$DISTFILE" 2>/dev/null || stat -f %z "$DISTFILE")

python3 - "$PORTFILE" "$SETUP" "$VERSION" "$REVISION" "$RMD160" "$SHA256" "$SIZE" <<'PY'
import re, sys
path, setup, version, revision, rmd160, sha256, size = sys.argv[1:8]
s = open(path).read()
def one(pattern, repl):
    global s
    s, n = re.subn(pattern, repl, s, count=1, flags=re.M)
    assert n == 1, pattern
one(r'^github\.setup[ \t]+[^\n]*$', setup)
one(r'^version[ \t]+[^\n]*$', 'version             %s' % version)
one(r'^revision[ \t]+[^\n]*$', 'revision            %s' % revision)
one(r'^checksums[ \t]+\$\{distname\}\$\{extract\.suffix\} \\\n(?:[ \t]+(?:rmd160|sha256|size)[^\n]*\n)+',
    'checksums           ${distname}${extract.suffix} \\\n'
    '                    rmd160  %s \\\n'
    '                    sha256  %s \\\n'
    '                    size    %s\n' % (rmd160, sha256, size))
open(path, 'w').write(s)
PY
cp "$PORTFILE" "$OUT/kate-qt4/Portfile"
echo "source:   $URL"
echo "commit:   $SHA (archive content verified)"
echo "version:  $VERSION, revision $REVISION"
echo "distfile: $DISTFILE ($SIZE bytes)"
echo "Portfile: $PORTFILE (copied to $OUT/kate-qt4/Portfile)"
