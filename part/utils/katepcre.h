/* This file is part of the KDE project
   Copyright (C) 2026 The Kate qt4-pure port

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KATE_PCRE_H
#define KATE_PCRE_H

#include <QtCore/QFlags>
#include <QtCore/QString>
#include <QtCore/QStringList>

class KatePcrePrivate;

/**
 * Perl-compatible regular expressions for KatePart, on pcre2's 16-bit
 * library so QString text is matched in place.
 *
 * This is the regex engine behind syntax highlighting (RegExpr rules), the
 * document search (KateRegExp, i.e. the search bar, vi mode and
 * KTextEditor::SearchInterface) and the search plugin. The upstream KDE
 * syntax-highlighting framework matches with QRegularExpression, which is
 * pcre2 as well, so the shipped highlighting definitions behave exactly as
 * upstream: patterns are compiled in UTF mode with Unicode properties
 * (\\w, \\d, \\b are Unicode aware), the newline is LF, "^" matches only at
 * offset 0 of the subject unless MultiLine is set.
 *
 * Compiled patterns are JIT compiled when the library supports it and fall
 * back to the interpreter otherwise (also when a match runs out of JIT
 * stack). The compiled code is shared between copies of a KatePcre; each
 * copy has its own match state, so threads may copy a pattern and match
 * concurrently.
 *
 * The matching API follows QRegExp's, which the existing search code was
 * written against: indexIn()/lastIndexIn() search and remember the match,
 * matchedLength()/pos()/cap()/capturedTexts() read it back.
 */
class KatePcre
{
  public:
    enum Option
    {
      NoOptions         = 0x00,
      CaseInsensitive   = 0x01,
      /** quantifiers are lazy by default (QRegExp::setMinimal, upstream "minimal") */
      Minimal           = 0x02,
      /** "^" and "$" also match right after / before a newline in the subject */
      MultiLine         = 0x04,
      /** "." also matches a newline */
      DotMatchesNewline = 0x08,
      /** plain parentheses do not capture (cheaper when captures are never read) */
      NoAutoCapture     = 0x10
    };
    Q_DECLARE_FLAGS(Options, Option)

    KatePcre();
    explicit KatePcre(const QString &pattern, Options options = NoOptions);
    KatePcre(const KatePcre &other);
    KatePcre &operator=(const KatePcre &other);
    ~KatePcre();

    /** compiles a new pattern; the previous match state is dropped */
    void setPattern(const QString &pattern, Options options = NoOptions);

    QString pattern() const;
    Options options() const;

    /** true for an empty pattern (which is invalid too) */
    bool isEmpty() const;
    /** false if the pattern did not compile; see errorString() */
    bool isValid() const;
    QString errorString() const;
    /** offset in pattern() of the compile error, -1 if valid */
    int errorOffset() const;
    /** number of capturing groups in the pattern (0 if invalid) */
    int captureCount() const;

    /**
     * Searches text[offset, end) for the first match and remembers it.
     * The whole of text is the subject (lookbehind can see text before
     * offset), but nothing at or after end is visible to the pattern, as
     * with QRegExp::indexIn(text.left(end), offset).
     * @param end   subject end, -1 for text.length()
     * @return index of the match or -1
     */
    int indexIn(const QString &text, int offset = 0, int end = -1) const;

    /**
     * The last match that a forward search from offset would find (this is
     * what "find previous" wants; QRegExp::lastIndexIn is subtly different).
     * @return index of the match or -1
     */
    int lastIndexIn(const QString &text, int offset = 0, int end = -1) const;

    /** true if the pattern matches exactly at offset; remembers the match */
    bool matchesAt(const QString &text, int offset, int end = -1) const;

    /**
     * true if the pattern matches the whole of text (anchored at both ends,
     * so alternations are tried until one spans it - QRegExp::exactMatch
     * semantics); remembers the match
     */
    bool exactMatch(const QString &text) const;

    /** length of the last match, -1 if the last search failed */
    int matchedLength() const;
    /** start of capture nth (0 = whole match) of the last match, -1 if unset */
    int pos(int nth = 0) const;
    /** text of capture nth (0 = whole match) of the last match */
    QString cap(int nth = 0) const;
    /** all captures of the last match, index 0 is the whole match */
    QStringList capturedTexts() const;

    /** escapes every character that has a meaning in a pattern */
    static QString escape(const QString &text);
    /** pcre2 version, for the about dialog / debugging */
    static QString versionString();
    /** whether patterns are JIT compiled on this machine */
    static bool hasJit();

  private:
    KatePcrePrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KatePcre::Options)

#endif

// kate: space-indent on; indent-width 2; replace-tabs on;
