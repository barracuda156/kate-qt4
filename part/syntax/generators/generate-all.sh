#!/bin/sh
# part/syntax/generators/generate-all.sh <outdir>
#
# Runs every generator on the definitions in ../data exactly as
# part/syntax/data/CMakeLists.txt does at build time: the PHP and Twig
# variants of the web languages, DoxygenLua, XSLT, JSONC (Perl) and the
# Jinja template variants (Python 3). For validating a definition snapshot
# without a build: generate into a scratch directory, then run
#   qt4compat/tools/validate-syntax.py part/syntax/data <outdir>
# PERL and PYTHON override the interpreters.
set -eu
[ $# -eq 1 ] || { echo "usage: $0 <outdir>" >&2; exit 2; }
OUT=$1
HERE=$(cd "$(dirname "$0")" && pwd)
DATA=$HERE/../data
PERL=${PERL:-perl}
PYTHON=${PYTHON:-python3}
mkdir -p "$OUT"
for lang in html css javascript javascript-react typescript mustache; do
    $PERL "$HERE/generate-html.pl" "$DATA/$lang.xml" "$OUT/$lang-php.xml"
done
for lang in html css javascript javascript-react typescript; do
    $PERL "$HERE/generate-html.pl" "$DATA/$lang.xml" "$OUT/$lang-twig.xml"
done
$PERL "$HERE/generate-doxygenlua.pl" "$DATA/doxygen.xml" "$OUT/doxygenlua.xml"
$PERL "$HERE/generate-xslt.pl" "$DATA/xml.xml" "$OUT/xslt.xml"
$PERL "$HERE/generate-jsonc.pl" "$DATA/json.xml" "$OUT/jsonc.xml"
inputs=""
for lang in html css xml json yaml toml sql python cpp qml java groovy rust javascript typescript ruby cmake qmake ini bash zsh dockerfile terraform systemd-unit; do
    inputs="$inputs $DATA/$lang.xml"
done
# shellcheck disable=SC2086
$PYTHON "$HERE/generate_jinja.py" --jinja-xml "$DATA/jinja.xml" --prefix jinja- --output-dir "$OUT" $inputs
echo "generated $(ls "$OUT" | wc -l | tr -d ' ') definitions in $OUT"
