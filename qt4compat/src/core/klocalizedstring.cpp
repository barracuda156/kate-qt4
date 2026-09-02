/* qt4compat: klocalizedstring.cpp - English-only KLocalizedString */
#include <klocalizedstring.h>
#include <klocale.h>

#include <QtCore/QStringList>
#include <QtCore/QVector>

class KLocalizedStringPrivate
{
public:
    KLocalizedStringPrivate() : hasPlural(false), empty(true), numberSet(false), number(0) {}
    QString ctxt;
    QString msg;
    QString plural;
    bool hasPlural;
    bool empty;
    QStringList args;
    bool numberSet;
    qlonglong number;
};

KLocalizedString::KLocalizedString()
    : d(new KLocalizedStringPrivate)
{
}

KLocalizedString::KLocalizedString(const char *ctxt, const char *msg, const char *plural)
    : d(new KLocalizedStringPrivate)
{
    if (ctxt)
        d->ctxt = QString::fromUtf8(ctxt);
    if (msg) {
        d->msg = QString::fromUtf8(msg);
        d->empty = false;
    }
    if (plural) {
        d->plural = QString::fromUtf8(plural);
        d->hasPlural = true;
    }
}

KLocalizedString::KLocalizedString(const KLocalizedString &rhs)
    : d(new KLocalizedStringPrivate(*rhs.d))
{
}

KLocalizedString &KLocalizedString::operator=(const KLocalizedString &rhs)
{
    if (&rhs != this)
        *d = *rhs.d;
    return *this;
}

KLocalizedString::~KLocalizedString()
{
    delete d;
}

bool KLocalizedString::isEmpty() const
{
    return d->empty;
}

static QString substitute(const QString &text, const QStringList &args)
{
    QString result;
    result.reserve(text.length() + 16);
    const int len = text.length();
    int i = 0;
    while (i < len) {
        const QChar c = text.at(i);
        if (c == QLatin1Char('%') && i + 1 < len && text.at(i + 1).isDigit()) {
            int j = i + 1;
            int num = 0;
            int digits = 0;
            while (j < len && text.at(j).isDigit() && digits < 2) {
                num = num * 10 + text.at(j).digitValue();
                ++j;
                ++digits;
            }
            /* prefer a single digit when the two-digit number has no argument */
            if (digits == 2 && num > args.count()) {
                num = text.at(i + 1).digitValue();
                j = i + 2;
            }
            if (num >= 1 && num <= args.count()) {
                result += args.at(num - 1);
                i = j;
                continue;
            }
        }
        result += c;
        ++i;
    }
    return result;
}

QString KLocalizedString::toString() const
{
    if (d->empty)
        return QString();
    QString text = d->msg;
    if (d->hasPlural && (!d->numberSet || d->number != 1))
        text = d->plural;
    return substitute(text, d->args);
}

QString KLocalizedString::toString(const QString &) const
{
    return toString();
}

QString KLocalizedString::toString(const KLocale *) const
{
    return toString();
}

QString KLocalizedString::toString(const KLocale *, const QString &) const
{
    return toString();
}

#define KLS_SUBS_NUMBER(EXPR, VALUE) \
    KLocalizedString kls(*this); \
    kls.d->args.append(EXPR); \
    if (!kls.d->numberSet) { kls.d->number = (VALUE); kls.d->numberSet = true; } \
    return kls;

KLocalizedString KLocalizedString::subs(int a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), a)
}

KLocalizedString KLocalizedString::subs(uint a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), a)
}

KLocalizedString KLocalizedString::subs(long a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), a)
}

KLocalizedString KLocalizedString::subs(ulong a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), (qlonglong)a)
}

KLocalizedString KLocalizedString::subs(qlonglong a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), a)
}

KLocalizedString KLocalizedString::subs(qulonglong a, int fieldWidth, int base, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, base, fillChar), (qlonglong)a)
}

KLocalizedString KLocalizedString::subs(double a, int fieldWidth, char format, int precision, const QChar &fillChar) const
{
    KLS_SUBS_NUMBER(QString::fromLatin1("%1").arg(a, fieldWidth, format, precision, fillChar), (qlonglong)a)
}

#undef KLS_SUBS_NUMBER

KLocalizedString KLocalizedString::subs(QChar a, int fieldWidth, const QChar &fillChar) const
{
    KLocalizedString kls(*this);
    kls.d->args.append(QString::fromLatin1("%1").arg(a, fieldWidth, fillChar));
    return kls;
}

KLocalizedString KLocalizedString::subs(const QString &a, int fieldWidth, const QChar &fillChar) const
{
    KLocalizedString kls(*this);
    kls.d->args.append(QString::fromLatin1("%1").arg(a, fieldWidth, fillChar));
    return kls;
}

KLocalizedString KLocalizedString::inContext(const QString &, const QString &) const
{
    return *this;
}

void KLocalizedString::notifyCatalogsUpdated(const QStringList &, const QList<QByteArray> &)
{
}

KLocalizedString ki18n(const char *msg)
{
    return KLocalizedString(0, msg, 0);
}

KLocalizedString ki18nc(const char *ctxt, const char *msg)
{
    return KLocalizedString(ctxt, msg, 0);
}

KLocalizedString ki18np(const char *singular, const char *plural)
{
    return KLocalizedString(0, singular, plural);
}

KLocalizedString ki18ncp(const char *ctxt, const char *singular, const char *plural)
{
    return KLocalizedString(ctxt, singular, plural);
}
