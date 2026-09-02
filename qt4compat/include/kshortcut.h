/* qt4compat: kshortcut.h - primary/alternate key sequence pair (kdelibs 4.14 API) */
#ifndef KDECOMPAT_KSHORTCUT_H
#define KDECOMPAT_KSHORTCUT_H

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QKeySequence>
#include <kdemacros.h>

class KDEUI_EXPORT KShortcut
{
public:
    enum EmptyHandling {
        KeepEmpty = 0,
        RemoveEmpty
    };

    KShortcut();
    explicit KShortcut(const QKeySequence &primary);
    KShortcut(const QKeySequence &primary, const QKeySequence &alternate);
    explicit KShortcut(int keyQtPri, int keyQtAlt = 0);
    KShortcut(const KShortcut &other);
    explicit KShortcut(const QString &description);
    explicit KShortcut(const QList<QKeySequence> &seqs);
    ~KShortcut();

    QKeySequence primary() const;
    QKeySequence alternate() const;
    bool isEmpty() const;
    bool contains(const QKeySequence &needle) const;
    bool conflictsWith(const QKeySequence &needle) const;
    QString toString() const;
    QString toString(QKeySequence::SequenceFormat format) const;
    bool operator==(const KShortcut &other) const;
    bool operator!=(const KShortcut &other) const;
    operator QList<QKeySequence>() const;
    QList<QKeySequence> toList(enum EmptyHandling handleEmpty = RemoveEmpty) const;
    operator QVariant() const;
    void setPrimary(const QKeySequence &keySeq);
    void setAlternate(const QKeySequence &keySeq);
    void remove(const QKeySequence &keySeq, enum EmptyHandling handleEmpty = RemoveEmpty);
    KShortcut &operator=(const KShortcut &other);

private:
    QKeySequence m_primary;
    QKeySequence m_alternate;
};

inline uint qHash(const QKeySequence &key)
{
    uint hash = 0;
    for (uint i = 0; i < key.count(); i++)
        hash += qHash(key[i]);
    return hash;
}

inline uint qHash(const KShortcut &key)
{
    return qHash(key.primary()[0]) + qHash(key.primary()[1]);
}

Q_DECLARE_METATYPE(KShortcut)

#endif
