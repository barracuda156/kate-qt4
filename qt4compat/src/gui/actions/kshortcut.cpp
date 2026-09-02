/* qt4compat: kshortcut.cpp */
#include <kshortcut.h>

#include <QtCore/QStringList>

KShortcut::KShortcut()
{
}

KShortcut::KShortcut(const QKeySequence &primary)
    : m_primary(primary)
{
}

KShortcut::KShortcut(const QKeySequence &primary, const QKeySequence &alternate)
    : m_primary(primary), m_alternate(alternate)
{
}

KShortcut::KShortcut(int keyQtPri, int keyQtAlt)
    : m_primary(keyQtPri), m_alternate(keyQtAlt)
{
}

KShortcut::KShortcut(const KShortcut &other)
    : m_primary(other.m_primary), m_alternate(other.m_alternate)
{
}

KShortcut::KShortcut(const QString &description)
{
    if (description.isEmpty())
        return;
    const QStringList cuts = description.split(QLatin1String("; "));
    if (cuts.count() >= 1)
        m_primary = QKeySequence::fromString(cuts.at(0), QKeySequence::PortableText);
    if (cuts.count() >= 2)
        m_alternate = QKeySequence::fromString(cuts.at(1), QKeySequence::PortableText);
}

KShortcut::KShortcut(const QList<QKeySequence> &seqs)
{
    if (seqs.count() >= 1)
        m_primary = seqs.at(0);
    if (seqs.count() >= 2)
        m_alternate = seqs.at(1);
}

KShortcut::~KShortcut()
{
}

QKeySequence KShortcut::primary() const
{
    return m_primary;
}

QKeySequence KShortcut::alternate() const
{
    return m_alternate;
}

bool KShortcut::isEmpty() const
{
    return m_primary.isEmpty() && m_alternate.isEmpty();
}

bool KShortcut::contains(const QKeySequence &needle) const
{
    if (needle.isEmpty())
        return false;
    return m_primary == needle || m_alternate == needle;
}

bool KShortcut::conflictsWith(const QKeySequence &needle) const
{
    if (needle.isEmpty())
        return false;
    bool primaryConflicts = !m_primary.isEmpty()
        && (m_primary.matches(needle) == QKeySequence::NoMatch ? (needle.matches(m_primary) != QKeySequence::NoMatch) : true);
    bool alternateConflicts = !m_alternate.isEmpty()
        && (m_alternate.matches(needle) == QKeySequence::NoMatch ? (needle.matches(m_alternate) != QKeySequence::NoMatch) : true);
    return primaryConflicts || alternateConflicts;
}

QString KShortcut::toString() const
{
    return toString(QKeySequence::PortableText);
}

QString KShortcut::toString(QKeySequence::SequenceFormat format) const
{
    QString ret;
    if (!m_primary.isEmpty())
        ret = m_primary.toString(format);
    if (!m_alternate.isEmpty()) {
        if (!ret.isEmpty())
            ret += QLatin1String("; ");
        ret += m_alternate.toString(format);
    }
    return ret;
}

bool KShortcut::operator==(const KShortcut &other) const
{
    return m_primary == other.m_primary && m_alternate == other.m_alternate;
}

bool KShortcut::operator!=(const KShortcut &other) const
{
    return !operator==(other);
}

KShortcut::operator QList<QKeySequence>() const
{
    return toList(RemoveEmpty);
}

QList<QKeySequence> KShortcut::toList(enum EmptyHandling handleEmpty) const
{
    QList<QKeySequence> ret;
    if (handleEmpty == RemoveEmpty) {
        if (!m_primary.isEmpty())
            ret.append(m_primary);
        if (!m_alternate.isEmpty())
            ret.append(m_alternate);
    } else {
        ret.append(m_primary);
        ret.append(m_alternate);
    }
    return ret;
}

KShortcut::operator QVariant() const
{
    return qVariantFromValue(*this);
}

void KShortcut::setPrimary(const QKeySequence &keySeq)
{
    m_primary = keySeq;
}

void KShortcut::setAlternate(const QKeySequence &keySeq)
{
    m_alternate = keySeq;
}

void KShortcut::remove(const QKeySequence &keySeq, enum EmptyHandling handleEmpty)
{
    if (keySeq.isEmpty())
        return;
    if (m_primary == keySeq) {
        if (handleEmpty == RemoveEmpty) {
            m_primary = m_alternate;
            m_alternate = QKeySequence();
        } else {
            m_primary = QKeySequence();
        }
    }
    if (m_alternate == keySeq)
        m_alternate = QKeySequence();
}

KShortcut &KShortcut::operator=(const KShortcut &other)
{
    m_primary = other.m_primary;
    m_alternate = other.m_alternate;
    return *this;
}
