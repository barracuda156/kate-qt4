#!/usr/bin/env python3
"""qt4compat/tools/gen-mimedata.py - generate src/io/kmimetype_data_p.h

Reads the shared-mime-info database (freedesktop.org.xml) and writes a
compact C++ table with the types a programmer's editor cares about:
all text/*, image/*, audio/*, video/*, font/*, inode/*, message/* and
model/* types, plus application/* types that are subclasses of text/plain
or application/xml (except vendor formats), have a text-like generic icon,
or are listed explicitly below. Only English comments are kept.

usage: gen-mimedata.py [/usr/share/mime/packages/freedesktop.org.xml] [out.h]
"""
import sys, xml.etree.ElementTree as ET

SRC = sys.argv[1] if len(sys.argv) > 1 else '/usr/share/mime/packages/freedesktop.org.xml'
OUT = sys.argv[2] if len(sys.argv) > 2 else 'src/io/kmimetype_data_p.h'
NS = '{http://www.freedesktop.org/standards/shared-mime-info}'
XMLLANG = '{http://www.w3.org/XML/1998/namespace}lang'

ALWAYS = set('''
application/octet-stream application/x-executable application/x-sharedlib
application/x-object application/x-core application/x-pie-executable
application/x-mach-binary application/x-ms-dos-executable application/x-msdownload
application/x-ms-ne-executable application/vnd.microsoft.portable-executable
application/pdf application/postscript application/x-dvi application/rtf
application/zip application/gzip application/x-bzip application/x-bzip2 application/bzip2
application/x-tar application/x-compressed-tar application/x-bzip-compressed-tar
application/x-bzip2-compressed-tar application/x-xz application/x-xz-compressed-tar
application/x-lzma application/x-lzma-compressed-tar application/x-lzip application/x-lz4
application/zstd application/x-zstd-compressed-tar application/x-7z-compressed
application/vnd.rar application/x-rar application/x-cpio application/x-archive
application/x-ar application/x-deb application/vnd.debian.binary-package application/x-rpm
application/x-java application/x-java-archive application/java-archive
application/vnd.sqlite3 application/x-sqlite3 application/x-sqlite2
application/x-shockwave-flash application/x-desktop application/x-yaml application/yaml
application/x-tex application/x-perl application/x-php application/x-ruby application/x-python
application/x-python-bytecode application/javascript application/json application/ld+json
application/xml application/xhtml+xml application/x-shellscript application/x-csh
application/x-awk application/x-lua application/x-tcl application/x-cmake
application/x-gettext-translation application/x-po application/x-designer application/x-qml
application/x-docbook+xml application/x-ipynb+json application/x-troff application/x-troff-man
application/x-x509-ca-cert application/pgp-encrypted application/pgp-signature application/pgp-keys
application/x-iso9660-image application/x-cd-image application/x-apple-diskimage
application/x-bittorrent application/x-trash application/x-zerosize application/x-empty
application/x-font-ttf application/x-font-otf application/x-font-type1 application/vnd.ms-fontobject
application/msword application/vnd.ms-excel application/vnd.ms-powerpoint
application/vnd.oasis.opendocument.text application/vnd.oasis.opendocument.spreadsheet
application/vnd.oasis.opendocument.presentation
application/vnd.openxmlformats-officedocument.wordprocessingml.document
application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
application/vnd.openxmlformats-officedocument.presentationml.presentation
application/x-sh application/x-bash application/x-zsh application/x-ksh
application/x-ms-shortcut application/x-mswinurl application/x-msi
application/x-apple-plist application/x-plist application/x-git application/x-subversion
application/sql application/x-sql application/x-diff application/x-patch
application/toml application/x-toml application/x-ini application/x-wine-extension-ini
application/x-mimearchive application/mbox application/x-tzo application/x-terminal
application/x-executable-script application/x-elf application/x-macbinary
application/x-nintendo-ds-rom application/x-doom-wad
'''.split())

TEXT_ICONS = set(['text-x-generic', 'text-x-script', 'text-html', 'text-x-generic-template',
                  'package-x-generic', 'application-x-executable', 'x-office-document',
                  'x-office-spreadsheet', 'x-office-presentation', 'application-certificate'])

root = ET.parse(SRC).getroot()
types = {}
aliases = {}
for m in root.findall(NS + 'mime-type'):
    name = m.get('type')
    comment = ''
    globs = []
    parents = []
    icon = ''
    for e in m:
        tag = e.tag.replace(NS, '')
        if tag == 'comment' and e.get(XMLLANG) is None:
            comment = (e.text or '').strip()
        elif tag == 'glob':
            w = int(e.get('weight', '50'))
            cs = e.get('case-sensitive', 'false') == 'true'
            globs.append((w, e.get('pattern'), cs))
        elif tag == 'sub-class-of':
            parents.append(e.get('type'))
        elif tag == 'alias':
            aliases[e.get('type')] = name
        elif tag == 'generic-icon':
            icon = e.get('name') or ''
    types[name] = dict(comment=comment, globs=globs, parents=parents, icon=icon)

def canonical(n):
    return aliases.get(n, n)

def is_subclass(n, of, seen=None):
    seen = seen or set()
    n = canonical(n)
    if n == of:
        return True
    if n in seen or n not in types:
        return False
    seen.add(n)
    return any(is_subclass(p, of, seen) for p in types[n]['parents'])

def wanted(n):
    major = n.split('/')[0]
    if major in ('text', 'image', 'audio', 'video', 'font', 'inode', 'message', 'model', 'multipart'):
        return True
    if n in ALWAYS:
        return True
    if is_subclass(n, 'text/plain'):
        return True
    if is_subclass(n, 'application/xml') and not n.startswith('application/vnd.'):
        return True
    if types[n]['icon'] in TEXT_ICONS and not n.startswith('application/vnd.'):
        return True
    return False

selected = sorted(n for n in types if wanted(n))
# synthetic entries for names the sniffer or Kate may use that the db lacks
synthetic = {
    'application/x-mach-binary': dict(comment='Mach-O binary', globs=[], parents=['application/octet-stream'], icon='application-x-executable'),
    'application/x-gzip': None,  # alias handled below
    'application/x-bzip': None,
    'application/x-empty': None,
}
for n, spec in synthetic.items():
    if n in types or n in aliases:
        continue
    if spec:
        types[n] = spec
        selected.append(n)
selected = sorted(set(selected))
sel = set(selected)
if 'application/x-gzip' not in aliases and 'application/x-gzip' not in types:
    aliases['application/x-gzip'] = 'application/gzip'
if 'application/x-bzip' not in aliases and 'application/x-bzip' not in types:
    aliases['application/x-bzip'] = 'application/x-bzip2'

def cstr(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'

out = []
out.append('/* qt4compat: generated by tools/gen-mimedata.py from shared-mime-info; do not edit.')
out.append(' * Source: freedesktop.org.xml (GPL-2.0-or-later data, http://freedesktop.org/wiki/Software/shared-mime-info) */')
out.append('#ifndef KDECOMPAT_KMIMETYPE_DATA_P_H')
out.append('#define KDECOMPAT_KMIMETYPE_DATA_P_H')
out.append('')
out.append('struct KMimeTypeEntry {')
out.append('    const char *name;')
out.append('    const char *comment;')
out.append('    const char *globs;    /* "weight:pattern" entries separated by |, weight is two digits, a leading ! marks case sensitive */')
out.append('    const char *parents;  /* | separated */')
out.append('    const char *icon;')
out.append('};')
out.append('')
out.append('static const KMimeTypeEntry kmime_entries[] = {')
for n in selected:
    t = types[n]
    globs = sorted(t['globs'], key=lambda g: (-g[0], -len(g[1])))
    gs = '|'.join('%s%02d:%s' % ('!' if cs else '', w, p) for (w, p, cs) in globs)
    ps = '|'.join(canonical(p) for p in t['parents'] if canonical(p) in sel or canonical(p) in types)
    out.append('    { %s, %s, %s, %s, %s },' % (cstr(n), cstr(t['comment']), cstr(gs), cstr(ps), cstr(t['icon'])))
out.append('};')
out.append('static const int kmime_entries_count = %d;' % len(selected))
out.append('')
out.append('struct KMimeAliasEntry { const char *alias; const char *canonical; };')
out.append('static const KMimeAliasEntry kmime_aliases[] = {')
n_alias = 0
for a in sorted(aliases):
    c = aliases[a]
    if c in sel:
        out.append('    { %s, %s },' % (cstr(a), cstr(c)))
        n_alias += 1
out.append('};')
out.append('static const int kmime_aliases_count = %d;' % n_alias)
out.append('')
out.append('#endif')
open(OUT, 'w').write('\n'.join(out) + '\n')
print('types:', len(selected), 'aliases:', n_alias, 'bytes:', len('\n'.join(out)))
