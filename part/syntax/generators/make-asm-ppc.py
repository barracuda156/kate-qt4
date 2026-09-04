#!/usr/bin/env python3
"""make-asm-ppc.py - build part/syntax/data/asm-ppc.xml from the BBEdit
Codeless Language Module for PowerPC assembler.

    make-asm-ppc.py ppcasm.plist > asm-ppc.xml

Source module: https://github.com/barracuda156/PPC_Assembler_BBEdit_CLM
(ppcasm.plist). Maps its instruction/register/directive keyword lists and
number/label patterns onto a native Kate syntax definition; ';' line
comments and both string kinds. The definition claims no file extension on
purpose (see part/syntax/data/local-changes.txt). Standard library only.
"""
import plistlib, sys, xml.sax.saxutils as sx

def dedup(seq):
    seen = set(); out = []
    for x in seq:
        if x not in seen:
            seen.add(x); out.append(x)
    return out

def items(lst):
    return '\n'.join('      <item>%s</item>' % sx.escape(x) for x in lst)

def main():
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    d = plistlib.load(open(sys.argv[1], 'rb'))
    kw = {e['RunKind'].split('.')[-1]: e['Keywords'] for e in d['BBLMKeywords']}
    instr = dedup(kw['instruction']); regs = dedup(kw['register']); direc = dedup(kw['directive'])
    tmpl = open(__file__.replace('make-asm-ppc.py', 'asm-ppc.xml.in')).read() \
        if False else TEMPLATE
    sys.stdout.write(tmpl % dict(instr=items(instr), regs=items(regs), direc=items(direc)))

TEMPLATE = r'''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE language SYSTEM "language.dtd">
<!--
  PowerPC Assembler highlighting for Kate.

  Translated from the BBEdit Codeless Language Module "PowerPC Assembler"
  (https://github.com/barracuda156/PPC_Assembler_BBEdit_CLM, ppcasm.plist) by
  part/syntax/generators/make-asm-ppc.py: its instruction/register/directive
  keyword lists and number/label patterns, ';' line comments, and both string
  kinds. No file extension is claimed on purpose (PowerPC assembly has none of
  its own and .s/.S default to the GNU Assembler definition) - select
  "PowerPC Assembler" from Tools > Highlighting. Part of the kate-qt4 port;
  see part/syntax/data/local-changes.txt.
-->
<language name="PowerPC Assembler" section="Assembler" version="1" kateversion="5.0" extensions="" casesensitive="1" author="PPC_Assembler_BBEdit_CLM (translated for Kate)" license="MIT">
  <highlighting>
    <list name="instructions">
%(instr)s
    </list>
    <list name="registers">
%(regs)s
    </list>
    <list name="directives">
%(direc)s
    </list>
    <contexts>
      <context name="Normal" attribute="Normal Text" lineEndContext="#stay">
        <DetectSpaces/>
        <RegExpr attribute="Label" String="^[a-zA-Z_][a-zA-Z0-9_]*:"/>
        <DetectChar attribute="Comment" context="Comment" char=";"/>
        <DetectChar attribute="String" context="StringDQ" char="&quot;"/>
        <DetectChar attribute="String" context="StringSQ" char="&apos;"/>
        <RegExpr attribute="Base-N Integer" String="0x[0-9a-fA-F]+"/>
        <RegExpr attribute="Base-N Integer" String="0b[01]+"/>
        <RegExpr attribute="Float" String="[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?"/>
        <RegExpr attribute="Base-N Integer" String="0[0-7]+"/>
        <RegExpr attribute="Decimal" String="[+-]?[1-9][0-9]*|0"/>
        <keyword attribute="Instruction" context="#stay" String="instructions" weakDeliminator=".+-"/>
        <keyword attribute="Directive" context="#stay" String="directives" weakDeliminator=".+-"/>
        <keyword attribute="Register" context="#stay" String="registers"/>
        <DetectIdentifier/>
      </context>
      <context name="Comment" attribute="Comment" lineEndContext="#pop">
        <DetectSpaces/>
        <DetectIdentifier/>
      </context>
      <context name="StringDQ" attribute="String" lineEndContext="#stay">
        <RegExpr attribute="String" String="\\."/>
        <DetectChar attribute="String" context="#pop" char="&quot;"/>
      </context>
      <context name="StringSQ" attribute="String" lineEndContext="#stay">
        <RegExpr attribute="String" String="\\."/>
        <DetectChar attribute="String" context="#pop" char="&apos;"/>
      </context>
    </contexts>
    <itemDatas>
      <itemData name="Normal Text"    defStyleNum="dsNormal"/>
      <itemData name="Instruction"    defStyleNum="dsKeyword"/>
      <itemData name="Directive"      defStyleNum="dsPreprocessor"/>
      <itemData name="Register"       defStyleNum="dsDataType"/>
      <itemData name="Label"          defStyleNum="dsFunction"/>
      <itemData name="Comment"        defStyleNum="dsComment"/>
      <itemData name="String"         defStyleNum="dsString"/>
      <itemData name="Decimal"        defStyleNum="dsDecVal"/>
      <itemData name="Base-N Integer" defStyleNum="dsBaseN"/>
      <itemData name="Float"          defStyleNum="dsFloat"/>
    </itemDatas>
  </highlighting>
  <general>
    <comments>
      <comment name="singleLine" start=";"/>
    </comments>
    <keywords casesensitive="1" weakDeliminator=".+-"/>
  </general>
</language>
'''

if __name__ == '__main__':
    main()
