/*  This file is part of the KDE libraries and the Kate part.
 *
 *  Copyright (C) 2009 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
 *  Copyright (C) 2007 Sebastian Pipping <webmaster@hartwork.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef _KATE_REGEXP_H_
#define _KATE_REGEXP_H_

#include "katepcre.h"

/**
 * Regular expression for the document search (search bar, vi mode,
 * KTextEditor::SearchInterface, the search plugin), on KatePcre. Beyond
 * plain matching it knows whether a pattern can match across lines and
 * repairs "." and "\\s" so they do not, as KateRegExpSearch needs.
 */
class KateRegExp
{
  public:
    explicit KateRegExp(const QString &pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    bool isEmpty() const { return m_pattern.isEmpty(); }
    bool isValid() const { return m_regExp.isValid(); }
    QString errorString() const { return m_regExp.errorString(); }
    QString pattern() const { return m_pattern; }
    int numCaptures() const { return m_regExp.captureCount(); }
    int pos(int nth = 0) const { return m_regExp.pos(nth); }
    QString cap(int nth = 0) const { return m_regExp.cap(nth); }
    QStringList capturedTexts() const { return m_regExp.capturedTexts(); }
    int matchedLength() const { return m_regExp.matchedLength(); }

    /**
     * First match in str[offset, end). "^" only matches at 0 (at line
     * starts too for a multi-line pattern, see repairPattern()).
     */
    int indexIn(const QString &str, int offset, int end) const;

    /**
     * The last match that a forward search from offset would find. We need
     * this behavior to allow the user to jump back to the last match.
     *
     * \param str        Text to search in
     * \param offset     Start of the search
     * \param end        End of the searched text
     * \return           Index of match or -1 if no match is found
     */
    int lastIndexIn(const QString &str, int offset, int end) const;

    /**
     * Repairs a regular Expression pattern.
     * This is a workaround to make "." and "\\s" not match
     * newlines, which is the default in a Perl regular expression.
     * A pattern that still matches across lines afterwards is
     * recompiled in multi-line mode, so "^" and "$" match at every
     * line boundary of the joined text it is run on.
     *
     * \param stillMultiLine  Multi-line after reparation flag
     * \return                Number of replacements done
     */
    int repairPattern(bool & stillMultiLine);

    /**
     * States, whether the pattern matches multiple lines,
     * even if it was repaired using @p repairPattern().
     *
     * \return Whether the pattern matches multiple lines
     */
    bool isMultiLine() const;

  private:
    void compile();

    QString m_pattern;
    Qt::CaseSensitivity m_caseSensitivity;
    bool m_multiLine;
    KatePcre m_regExp;
};

#endif // KATEREGEXP_H

// kate: space-indent on; indent-width 2; replace-tabs on;
