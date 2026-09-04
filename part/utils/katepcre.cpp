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

#include "katepcre.h"

#include <kdebug.h>

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QSharedData>
#include <QtCore/QVector>

#define PCRE2_CODE_UNIT_WIDTH 16
#include <pcre2.h>

/**
 * The compiled pattern, shared between copies of a KatePcre. Immutable after
 * construction (pcre2 codes may be used from several threads at once), so
 * sharing needs no lock beyond the atomic reference count.
 */
class KatePcreCode : public QSharedData
{
  public:
    KatePcreCode(const QString &pattern, KatePcre::Options options);
    ~KatePcreCode();

    QString pattern;
    KatePcre::Options options;
    pcre2_code *code;
    bool valid;
    bool jit;
    QString error;
    int errorOffset;
    int captureCount;

  private:
    KatePcreCode(const KatePcreCode &);
    KatePcreCode &operator=(const KatePcreCode &);
};

static pcre2_compile_context *compileContext()
{
  // LF is what the buffer uses between joined lines; do not depend on the
  // newline convention pcre2 was built with. Created once; patterns may be
  // compiled from search worker threads, hence the lock.
  static QMutex mutex;
  static pcre2_compile_context *ctx = 0;
  QMutexLocker locker(&mutex);
  if (!ctx)
  {
    ctx = pcre2_compile_context_create(0);
    if (ctx)
      pcre2_set_newline(ctx, PCRE2_NEWLINE_LF);
  }
  return ctx;
}

static bool jitAvailable()
{
  static int available = -1;
  if (available < 0)
  {
    uint32_t jit = 0;
    available = (pcre2_config(PCRE2_CONFIG_JIT, &jit) >= 0 && jit) ? 1 : 0;
  }
  return available == 1;
}

KatePcreCode::KatePcreCode(const QString &p, KatePcre::Options o)
  : pattern(p)
  , options(o)
  , code(0)
  , valid(false)
  , jit(false)
  , errorOffset(-1)
  , captureCount(0)
{
  if (pattern.isEmpty())
  {
    error = QLatin1String("empty pattern");
    return;
  }

  uint32_t flags = PCRE2_UTF | PCRE2_UCP;
  if (options & KatePcre::CaseInsensitive)
    flags |= PCRE2_CASELESS;
  if (options & KatePcre::Minimal)
    flags |= PCRE2_UNGREEDY;
  if (options & KatePcre::MultiLine)
    flags |= PCRE2_MULTILINE;
  if (options & KatePcre::DotMatchesNewline)
    flags |= PCRE2_DOTALL;
  if (options & KatePcre::NoAutoCapture)
    flags |= PCRE2_NO_AUTO_CAPTURE;

  int errorCode = 0;
  PCRE2_SIZE errorPos = 0;
  code = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern.utf16()), pattern.length(),
                       flags, &errorCode, &errorPos, compileContext());
  if (!code)
  {
    PCRE2_UCHAR buffer[256];
    if (pcre2_get_error_message(errorCode, buffer, sizeof(buffer) / sizeof(buffer[0])) >= 0)
      error = QString::fromUtf16(reinterpret_cast<const ushort *>(buffer));
    else
      error = QString::fromLatin1("pcre2 error %1").arg(errorCode);
    errorOffset = int(errorPos);
    return;
  }

  valid = true;

  uint32_t count = 0;
  if (pcre2_pattern_info(code, PCRE2_INFO_CAPTURECOUNT, &count) == 0)
    captureCount = int(count);

  // JIT: a failure here (unsupported CPU, out of memory) just means the
  // interpreter is used; pcre2_match() picks the JIT code automatically
  if (jitAvailable() && pcre2_jit_compile(code, PCRE2_JIT_COMPLETE) == 0)
    jit = true;
}

KatePcreCode::~KatePcreCode()
{
  if (code)
    pcre2_code_free(code);
}

/**
 * Per-object match state: the match data block and the last match.
 */
class KatePcrePrivate
{
  public:
    KatePcrePrivate()
      : matchData(0)
      , matchStart(-1)
      , matchLength(-1)
    {}

    ~KatePcrePrivate()
    {
      if (matchData)
        pcre2_match_data_free(matchData);
    }

    void resetCode(const QExplicitlySharedDataPointer<KatePcreCode> &newCode)
    {
      code = newCode;
      if (matchData)
      {
        pcre2_match_data_free(matchData);
        matchData = 0;
      }
      clearMatch();
    }

    void clearMatch()
    {
      matchStart = -1;
      matchLength = -1;
      subject.clear();
      groups.clear();
    }

    bool ensureMatchData()
    {
      if (matchData)
        return true;
      if (!code || !code->valid)
        return false;
      matchData = pcre2_match_data_create_from_pattern(code->code, 0);
      return matchData != 0;
    }

    /**
     * Runs the pattern on text[0, end) starting at offset; on success the
     * match is remembered and its start returned, otherwise -1.
     */
    int match(const QString &text, int offset, int end, uint32_t flags)
    {
      clearMatch();
      if (!ensureMatchData())
        return -1;

      if (end < 0 || end > text.length())
        end = text.length();
      if (offset < 0)
        offset = 0;
      if (offset > end)
        return -1;

      int rc = pcre2_match(code->code, reinterpret_cast<PCRE2_SPTR>(text.utf16()), end, offset,
                           flags, matchData, 0);
      if (rc == PCRE2_ERROR_JIT_STACKLIMIT)
      {
        // pattern too deep for the default JIT stack: the interpreter has no such limit
        rc = pcre2_match(code->code, reinterpret_cast<PCRE2_SPTR>(text.utf16()), end, offset,
                         flags | PCRE2_NO_JIT, matchData, 0);
      }
      if (rc < 0)
      {
        if (rc != PCRE2_ERROR_NOMATCH)
          kDebug(13010) << "pcre2_match failed with" << rc << "for pattern" << code->pattern;
        return -1;
      }

      // rc == 0 cannot happen: the match data was sized from the pattern
      const PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(matchData);
      const int pairs = int(pcre2_get_ovector_count(matchData));
      groups.resize(pairs);
      for (int i = 0; i < pairs; ++i)
      {
        const PCRE2_SIZE s = ovector[2 * i];
        const PCRE2_SIZE e = ovector[2 * i + 1];
        if (s == PCRE2_UNSET || e == PCRE2_UNSET)
        {
          groups[i].start = -1;
          groups[i].length = -1;
        }
        else
        {
          groups[i].start = int(s);
          groups[i].length = int(e - s);
        }
      }
      subject = text;
      matchStart = groups[0].start;
      matchLength = groups[0].length;
      return matchStart;
    }

    struct Group
    {
      int start;
      int length;
    };

    QExplicitlySharedDataPointer<KatePcreCode> code;
    pcre2_match_data *matchData;
    int matchStart;
    int matchLength;
    QString subject;          // implicitly shared, cheap to keep
    QVector<Group> groups;    // index 0 is the whole match
};

KatePcre::KatePcre()
  : d(new KatePcrePrivate)
{
}

KatePcre::KatePcre(const QString &pattern, Options options)
  : d(new KatePcrePrivate)
{
  setPattern(pattern, options);
}

KatePcre::KatePcre(const KatePcre &other)
  : d(new KatePcrePrivate)
{
  d->resetCode(other.d->code);
}

KatePcre &KatePcre::operator=(const KatePcre &other)
{
  if (this != &other)
    d->resetCode(other.d->code);
  return *this;
}

KatePcre::~KatePcre()
{
  delete d;
}

void KatePcre::setPattern(const QString &pattern, Options options)
{
  d->resetCode(QExplicitlySharedDataPointer<KatePcreCode>(new KatePcreCode(pattern, options)));
}

QString KatePcre::pattern() const
{
  return d->code ? d->code->pattern : QString();
}

KatePcre::Options KatePcre::options() const
{
  return d->code ? d->code->options : Options(NoOptions);
}

bool KatePcre::isEmpty() const
{
  return !d->code || d->code->pattern.isEmpty();
}

bool KatePcre::isValid() const
{
  return d->code && d->code->valid;
}

QString KatePcre::errorString() const
{
  if (!d->code)
    return QLatin1String("empty pattern");
  return d->code->error;
}

int KatePcre::errorOffset() const
{
  return d->code ? d->code->errorOffset : -1;
}

int KatePcre::captureCount() const
{
  return isValid() ? d->code->captureCount : 0;
}

int KatePcre::indexIn(const QString &text, int offset, int end) const
{
  return d->match(text, offset, end, 0);
}

int KatePcre::lastIndexIn(const QString &text, int offset, int end) const
{
  if (end < 0 || end > text.length())
    end = text.length();

  int last = -1;
  int lastLength = 0;
  int pos = d->match(text, offset, end, 0);
  while (pos != -1)
  {
    last = pos;
    lastLength = d->matchLength;
    // step past the match; an empty match still has to advance
    const int next = pos + (lastLength > 0 ? lastLength : 1);
    if (next > end)
      break;
    pos = d->match(text, next, end, 0);
  }

  if (last == -1)
  {
    d->clearMatch();
    return -1;
  }

  // restore the state of the last match (the search from 'last' finds it again)
  return d->match(text, last, end, 0);
}

bool KatePcre::matchesAt(const QString &text, int offset, int end) const
{
  return d->match(text, offset, end, PCRE2_ANCHORED) == offset;
}

bool KatePcre::exactMatch(const QString &text) const
{
  return d->match(text, 0, -1, PCRE2_ANCHORED | PCRE2_ENDANCHORED) == 0
         && d->matchLength == text.length();
}

int KatePcre::matchedLength() const
{
  return d->matchLength;
}

int KatePcre::pos(int nth) const
{
  if (nth < 0 || nth >= d->groups.size())
    return -1;
  return d->groups.at(nth).start;
}

QString KatePcre::cap(int nth) const
{
  if (nth < 0 || nth >= d->groups.size())
    return QString();
  const KatePcrePrivate::Group &g = d->groups.at(nth);
  if (g.start < 0)
    return QString();
  return d->subject.mid(g.start, g.length);
}

QStringList KatePcre::capturedTexts() const
{
  QStringList list;
  if (d->matchStart < 0)
    return list;
  // captureCount() + 1 entries, unset groups are empty, like QRegExp
  const int count = captureCount() + 1;
  for (int i = 0; i < count; ++i)
    list << cap(i);
  return list;
}

QString KatePcre::escape(const QString &text)
{
  // everything but [A-Za-z0-9_] gets a backslash, as QRegularExpression::escape does;
  // NUL is spelled out, a backslash followed by NUL would not survive
  QString result;
  result.reserve(text.length() * 2);
  for (int i = 0; i < text.length(); ++i)
  {
    const QChar c = text.at(i);
    const ushort u = c.unicode();
    if ((u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z') || (u >= '0' && u <= '9') || u == '_')
      result.append(c);
    else if (u == 0)
      result.append(QLatin1String("\\0"));
    else
    {
      result.append(QLatin1Char('\\'));
      result.append(c);
    }
  }
  return result;
}

QString KatePcre::versionString()
{
  PCRE2_UCHAR buffer[64];
  if (pcre2_config(PCRE2_CONFIG_VERSION, buffer) < 0)
    return QString();
  return QString::fromUtf16(reinterpret_cast<const ushort *>(buffer));
}

bool KatePcre::hasJit()
{
  return jitAvailable();
}

// kate: space-indent on; indent-width 2; replace-tabs on;
