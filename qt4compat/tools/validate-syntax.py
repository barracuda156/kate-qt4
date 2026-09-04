#!/usr/bin/env python3
"""validate-syntax.py - validate Kate syntax-highlighting XML definitions.

Validates every *.xml definition under part/syntax/data (or the files /
directories given on the command line):

  a. XML well-formedness; root element must be <language name="...">;
     duplicate language names across files are reported.
  b. Every <RegExpr String="..."> pattern must compile with pcre2 in 16-bit
     UTF+UCP mode (using the pcre2test binary).
  c. Cross references: context="...", lineEndContext=, lineEmptyContext=,
     fallthroughContext= (on any element, including <IncludeRules>) and
     <include> entries inside <list> must resolve to a real context/list/
     language.
  d. <language indenter="..."> must name a script in
     part/script/data/indentation/*.js (reported as a warning, not a
     failure).

Prints one line per problem plus a summary; exits 1 if any check (a)-(c)
problem was found (check (d) issues are warnings only).

Only the Python standard library is used.
"""

from __future__ import print_function

import argparse
import glob
import os
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "..", ".."))
DEFAULT_DATA_DIR = os.path.join(REPO_ROOT, "part", "syntax", "data")
INDENTATION_DIR = os.path.join(REPO_ROOT, "part", "script", "data", "indentation")

DEFAULT_PCRE2TEST = "/home/sgr/barracuda/deproot/usr/bin/pcre2test"
DEFAULT_PCRE2TEST_LIBDIR = "/home/sgr/barracuda/deproot/usr/lib/x86_64-linux-gnu"

# Candidate delimiter characters for pcre2test pattern lines, tried in
# order until one that does not occur in the pattern text is found.
# NOTE: '#' is deliberately excluded: pcre2test treats a line whose first
# character is '#' as a special command line (e.g. "#pattern", "#load"),
# not as a delimited pattern, even when '#' is otherwise a legal
# delimiter character - so a pattern beginning with a '#'-delimited
# pattern is misparsed as "** Unknown command: #...". Verified empirically.
DELIMITER_CANDIDATES = ['/', '!', '"', '%', '&', "'", '`', '~', '@', '=', ';', ',']

CONTEXT_REF_ATTRS = ('context', 'lineEndContext', 'lineEmptyContext', 'fallthroughContext')

POP_ONLY_RE = re.compile(r'^(?:#pop)+$')
DYNAMIC_PLACEHOLDER_RE = re.compile(r'%[1-9]')


def is_true(value):
    return value is not None and value.strip().lower() in ('1', 'true')


def read_raw_text(path):
    with open(path, 'rb') as f:
        data = f.read()
    try:
        return data.decode('utf-8')
    except UnicodeDecodeError:
        return data.decode('iso-8859-1')


def strip_xml_comments_keep_lines(text):
    """Blank out the interior of XML comments while preserving newlines,
    so line numbers stay correct and stray '<RegExpr' text inside a
    comment cannot be mistaken for a real element."""
    def blank(m):
        return re.sub(r'[^\n]', ' ', m.group(0))
    return re.sub(r'<!--.*?-->', blank, text, flags=re.DOTALL)


class FileInfo(object):
    def __init__(self, path):
        self.path = path
        self.xml_ok = False
        self.root = None
        self.language_name = None
        self.indenter = None
        self.context_names = set()
        self.list_names = set()
        self.raw_text = None


def collect_xml_files(paths):
    files = []
    for p in paths:
        if os.path.isdir(p):
            files.extend(sorted(glob.glob(os.path.join(p, '*.xml'))))
        elif os.path.isfile(p):
            files.append(p)
        else:
            print("%s: no such file or directory" % p, file=sys.stderr)
    # de-duplicate while preserving order
    seen = set()
    out = []
    for f in files:
        af = os.path.abspath(f)
        if af not in seen:
            seen.add(af)
            out.append(f)
    return out


def load_file(path, problems):
    info = FileInfo(path)
    try:
        info.raw_text = read_raw_text(path)
    except IOError as e:
        problems.append("%s: cannot read file: %s" % (path, e))
        return info

    try:
        tree = ET.parse(path)
    except ET.ParseError as e:
        problems.append("%s: XML parse error: %s" % (path, e))
        return info

    root = tree.getroot()
    info.root = root

    if root.tag != 'language':
        problems.append("%s: root element is <%s>, expected <language>" % (path, root.tag))
        return info

    name = root.get('name')
    if not name:
        problems.append("%s: <language> element is missing the 'name' attribute" % path)
        return info

    info.xml_ok = True
    info.language_name = name
    info.indenter = root.get('indenter')

    for ctx in root.iter('context'):
        cname = ctx.get('name')
        if cname:
            info.context_names.add(cname)

    for lst in root.iter('list'):
        lname = lst.get('name')
        if lname:
            info.list_names.add(lname)

    return info


def validate_context_ref(path, attr, value, local_contexts, all_language_names, problems):
    if value is None:
        return
    value = value.strip()
    if not value:
        problems.append("%s: empty %s=\"\" reference" % (path, attr))
        return

    if value == '#stay':
        return

    # The value is a "!"-separated chain of one or more context switches:
    # the first switches to (or pops to) a base context, and each further
    # segment pushes another context on top of it, e.g. "#pop!Foo",
    # "ValueSepOrRoot!Hash", or a deep chain like
    # "BlockPrefix!ValueTextIndent!ExplicitValueIndent!..." (all used
    # in-tree, see e.g. yaml.xml, mermaid.xml). Independently, any segment
    # (not just the first) may be a "##Language" or "Context##Language"
    # cross-file reference instead of a same-file context name, e.g.
    # "Arguments##Fish!FieldSep##Fish!CommandName##Fish" (see latex.xml).
    # Only the first segment may be a "#pop[#pop...]" sequence.
    segments = value.split('!')
    for i, seg in enumerate(segments):
        is_first = (i == 0)

        if is_first and seg.startswith('#pop'):
            if not POP_ONLY_RE.match(seg):
                problems.append(
                    "%s: %s=\"%s\" is not a valid #pop[...] context reference"
                    % (path, attr, value))
            continue

        if not seg:
            problems.append(
                "%s: %s=\"%s\" has no %s context name %s \"!\""
                % (path, attr, value,
                   'base' if is_first else 'pushed',
                   'before' if is_first else 'after'))
            continue

        if '##' in seg:
            lang = seg.split('##', 1)[1]
            if lang not in all_language_names:
                problems.append(
                    "%s: %s=\"%s\" references unknown language \"%s\""
                    % (path, attr, value, lang))
            continue

        if seg not in local_contexts:
            problems.append(
                "%s: %s=\"%s\" %s unknown context \"%s\" (not defined in this file)"
                % (path, attr, value,
                   'references' if is_first else 'pushes', seg))


def check_cross_references(info, all_language_names, language_to_lists, problems):
    path = info.path
    local_contexts = info.context_names
    # every element in the tree, generic scan for context-reference attributes
    # (covers rule elements' context=, <context>'s lineEndContext=/
    # lineEmptyContext=/fallthroughContext=, and <IncludeRules context=)
    for elem in info.root.iter():
        for attr in CONTEXT_REF_ATTRS:
            if attr in elem.attrib:
                validate_context_ref(path, attr, elem.get(attr), local_contexts,
                                      all_language_names, problems)

    # <include> inside <list>
    for lst in info.root.iter('list'):
        for inc in lst.findall('include'):
            text = (inc.text or '').strip()
            if not text:
                problems.append("%s: empty <include></include> inside <list name=\"%s\">"
                                 % (path, lst.get('name')))
                continue
            if '##' in text:
                list_name, lang = text.split('##', 1)
                if lang not in all_language_names:
                    problems.append(
                        "%s: <include>%s</include> references unknown language \"%s\""
                        % (path, text, lang))
                elif list_name not in language_to_lists.get(lang, set()):
                    problems.append(
                        "%s: <include>%s</include> references unknown list \"%s\" in language \"%s\""
                        % (path, text, list_name, lang))
            else:
                if text not in info.list_names:
                    problems.append(
                        "%s: <include>%s</include> references unknown list \"%s\" (not defined in this file)"
                        % (path, text, text))


def find_line_numbers(raw_text, tag):
    """Line number (1-based) of each '<tag' occurrence in raw_text, in
    document order, with the interior of XML comments blanked out."""
    stripped = strip_xml_comments_keep_lines(raw_text)
    lines = []
    for m in re.finditer(r'<%s\b' % re.escape(tag), stripped):
        lines.append(stripped.count('\n', 0, m.start()) + 1)
    return lines


def pick_delimiter(pattern):
    for c in DELIMITER_CANDIDATES:
        if c not in pattern:
            return c
    return None


def check_regexprs(info, pcre2test, pcre2test_env, problems, warnings, stats):
    path = info.path
    tree_elems = list(info.root.iter('RegExpr'))
    if not tree_elems:
        return

    raw_lines = find_line_numbers(info.raw_text, 'RegExpr')
    if len(raw_lines) != len(tree_elems):
        # lost the ability to reliably attribute line numbers; fall back
        # to "line unknown" for every RegExpr in this file
        raw_lines = [None] * len(tree_elems)

    batch = []  # list of dict: pattern, sent_line, line_no
    for elem, line_no in zip(tree_elems, raw_lines):
        pattern = elem.get('String')
        if pattern is None:
            problems.append("%s:%s: <RegExpr> element has no String attribute"
                             % (path, line_no if line_no else '?'))
            continue

        if is_true(elem.get('dynamic')):
            pattern = DYNAMIC_PLACEHOLDER_RE.sub('x', pattern)

        if '\n' in pattern or '\r' in pattern:
            stats['patterns_skipped'] += 1
            warnings.append(
                "%s:%s: RegExpr pattern contains an embedded literal newline; "
                "not checked by this validator: %r"
                % (path, line_no if line_no else '?', pattern))
            continue

        delim = pick_delimiter(pattern)
        if delim is None:
            # Extremely dense pattern that contains a bare occurrence of
            # every character pcre2test accepts as a delimiter. This is a
            # tooling limitation, not a defect in the pattern, so it is
            # reported as a warning rather than a failure.
            stats['patterns_skipped'] += 1
            warnings.append(
                "%s:%s: no pcre2test delimiter character is free in this pattern "
                "(it contains a bare occurrence of every legal delimiter); "
                "not checked by this validator: %r"
                % (path, line_no if line_no else '?', pattern))
            continue

        modifiers = ['utf', 'ucp']
        if is_true(elem.get('insensitive')):
            modifiers.append('caseless')
        if is_true(elem.get('minimal')):
            modifiers.append('ungreedy')

        sent_line = "%s%s%s%s" % (delim, pattern, delim, ','.join(modifiers))
        batch.append({'pattern': pattern, 'sent_line': sent_line, 'line_no': line_no})

    if not batch:
        return

    stdin_data = ''.join(b['sent_line'] + '\n\n' for b in batch)

    try:
        proc = subprocess.Popen([pcre2test, '-16', '-q'],
                                 stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE, env=pcre2test_env,
                                 universal_newlines=True)
        out, err = proc.communicate(stdin_data)
    except OSError as e:
        problems.append("%s: failed to run pcre2test: %s" % (path, e))
        return

    out_lines = out.split('\n')
    oi = 0
    for b in batch:
        stats['patterns_checked'] += 1
        if oi >= len(out_lines) or out_lines[oi] != b['sent_line']:
            problems.append(
                "%s:%s: internal error: lost sync with pcre2test output while "
                "checking pattern %r; remaining patterns in this file may not "
                "have been fully checked"
                % (path, b['line_no'] if b['line_no'] else '?', b['pattern']))
            break
        oi += 1

        failed_msg = None
        if oi < len(out_lines) and out_lines[oi].startswith('Failed:'):
            failed_msg = out_lines[oi]
            oi += 1
        if oi < len(out_lines) and out_lines[oi] == '':
            oi += 1

        if failed_msg:
            stats['patterns_failed'] += 1
            problems.append(
                "%s:%s: RegExpr pattern %r failed to compile with pcre2 (utf,ucp): %s"
                % (path, b['line_no'] if b['line_no'] else '?', b['pattern'], failed_msg))
        else:
            stats['patterns_ok'] += 1


def check_indenter(info, valid_indenters, indenter_missing):
    if not info.indenter:
        return
    if info.indenter in valid_indenters:
        return
    indenter_missing.setdefault(info.indenter, []).append(info.path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('paths', nargs='*', default=[DEFAULT_DATA_DIR],
                         help='XML files or directories to validate (default: %s)' % DEFAULT_DATA_DIR)
    parser.add_argument('--pcre2test', default=None,
                         help='path to the pcre2test binary (default: $PCRE2TEST or %s)' % DEFAULT_PCRE2TEST)
    args = parser.parse_args()

    pcre2test = args.pcre2test or os.environ.get('PCRE2TEST') or DEFAULT_PCRE2TEST
    if not (os.path.isfile(pcre2test) and os.access(pcre2test, os.X_OK)):
        print("error: pcre2test binary not found or not executable: %s" % pcre2test, file=sys.stderr)
        return 2

    pcre2test_env = os.environ.copy()
    existing_ld = pcre2test_env.get('LD_LIBRARY_PATH', '')
    if existing_ld:
        pcre2test_env['LD_LIBRARY_PATH'] = DEFAULT_PCRE2TEST_LIBDIR + os.pathsep + existing_ld
    else:
        pcre2test_env['LD_LIBRARY_PATH'] = DEFAULT_PCRE2TEST_LIBDIR

    files = collect_xml_files(args.paths)
    if not files:
        print("error: no XML files found in: %s" % ' '.join(args.paths), file=sys.stderr)
        return 2

    # "none" and "normal" are KatePart's built-in indentation modes, not scripts
    valid_indenters = set(['none', 'normal'])
    if os.path.isdir(INDENTATION_DIR):
        for f in glob.glob(os.path.join(INDENTATION_DIR, '*.js')):
            valid_indenters.add(os.path.splitext(os.path.basename(f))[0])

    problems = []

    # --- first pass: well-formedness + collect per-file metadata ---
    infos = []
    for path in files:
        info = load_file(path, problems)
        infos.append(info)

    ok_infos = [i for i in infos if i.xml_ok]

    # duplicate language names
    language_files = {}
    for info in ok_infos:
        language_files.setdefault(info.language_name, []).append(info.path)
    for name, plist in sorted(language_files.items()):
        if len(plist) > 1:
            problems.append("duplicate language name \"%s\" used by: %s"
                             % (name, ', '.join(sorted(plist))))

    all_language_names = set(language_files.keys())

    language_to_lists = {}
    for info in ok_infos:
        language_to_lists.setdefault(info.language_name, set()).update(info.list_names)

    # --- second pass: cross references, RegExpr/pcre2, indenter ---
    stats = {'patterns_checked': 0, 'patterns_ok': 0, 'patterns_failed': 0, 'patterns_skipped': 0}
    indenter_missing = {}
    warnings = []

    for info in ok_infos:
        check_cross_references(info, all_language_names, language_to_lists, problems)
        check_regexprs(info, pcre2test, pcre2test_env, problems, warnings, stats)
        check_indenter(info, valid_indenters, indenter_missing)

    for p in problems:
        print(p)

    for w in warnings:
        print("WARNING: " + w)

    if indenter_missing:
        for name, plist in sorted(indenter_missing.items()):
            print("WARNING: indenter \"%s\" has no matching script in %s (%s.js not found), "
                  "referenced by %d file(s): %s"
                  % (name, os.path.relpath(INDENTATION_DIR, REPO_ROOT), name, len(plist),
                     ', '.join(os.path.relpath(p, REPO_ROOT) for p in sorted(plist)[:5]) +
                     (', ...' if len(plist) > 5 else '')))

    n_files = len(files)
    n_ok = len(ok_infos)
    n_malformed = n_files - n_ok
    n_languages = len(all_language_names)

    print("")
    print("summary: %d files (%d parsed OK, %d malformed/rejected), %d languages, "
          "%d RegExpr patterns found (%d compiled ok, %d failed, %d skipped/unchecked), "
          "%d problem(s), %d regex warning(s), %d indenter warning(s)"
          % (n_files, n_ok, n_malformed, n_languages,
             stats['patterns_checked'] + stats['patterns_skipped'],
             stats['patterns_ok'], stats['patterns_failed'], stats['patterns_skipped'],
             len(problems), len(warnings), len(indenter_missing)))

    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
