/* qt4compat: klocalizedstring.h - KDE4-style i18n() on plain Qt4.
 * Messages are UTF-8 source strings; %1..%9 are substituted in order.
 * Plural forms use English rules (n == 1 -> singular). Translations are
 * not loaded (the whole application is English), but the API is complete so
 * that QTranslator-based catalogs can be wired in later. */
#ifndef KDECOMPAT_KLOCALIZEDSTRING_H
#define KDECOMPAT_KLOCALIZEDSTRING_H

#include <QtCore/QChar>
#include <QtCore/QLatin1Char>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <kglobal.h>

class KLocale;
class KLocalizedStringPrivate;

class KDECORE_EXPORT KLocalizedString
{
    friend KLocalizedString ki18n(const char *msg);
    friend KLocalizedString ki18nc(const char *ctxt, const char *msg);
    friend KLocalizedString ki18np(const char *singular, const char *plural);
    friend KLocalizedString ki18ncp(const char *ctxt, const char *singular, const char *plural);

public:
    KLocalizedString();
    KLocalizedString(const KLocalizedString &rhs);
    KLocalizedString &operator=(const KLocalizedString &rhs);
    ~KLocalizedString();

    QString toString() const;
    QString toString(const QString &catalogName) const;
    QString toString(const KLocale *locale) const;
    QString toString(const KLocale *locale, const QString &catalogName) const;

    bool isEmpty() const;

    KLocalizedString subs(int a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(uint a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(long a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(ulong a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(qlonglong a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(qulonglong a, int fieldWidth = 0, int base = 10, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(double a, int fieldWidth = 0, char format = 'g', int precision = -1, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(QChar a, int fieldWidth = 0, const QChar &fillChar = QLatin1Char(' ')) const;
    KLocalizedString subs(const QString &a, int fieldWidth = 0, const QChar &fillChar = QLatin1Char(' ')) const;

    KLocalizedString inContext(const QString &key, const QString &text) const;

    static void notifyCatalogsUpdated(const QStringList &languages, const QList<QByteArray> &catalogs);

private:
    KLocalizedString(const char *ctxt, const char *msg, const char *plural);
    KLocalizedStringPrivate *const d;
};

KDECORE_EXPORT KLocalizedString ki18n(const char *msg);
KDECORE_EXPORT KLocalizedString ki18nc(const char *ctxt, const char *msg);
KDECORE_EXPORT KLocalizedString ki18np(const char *singular, const char *plural);
KDECORE_EXPORT KLocalizedString ki18ncp(const char *ctxt, const char *singular, const char *plural);

#ifndef I18N_NOOP
#define I18N_NOOP(x) x
#endif
#ifndef I18N_NOOP2
#define I18N_NOOP2(comment,x) x
#endif
#ifndef I18N_NOOP2_NOSTRIP
#define I18N_NOOP2_NOSTRIP(comment,x) comment, x
#endif

inline QString tr2i18n(const char *message, const char *comment = 0)
{
    if (comment && comment[0] && message && message[0])
        return ki18nc(comment, message).toString();
    else if (message && message[0])
        return ki18n(message).toString();
    return QString();
}

/* i18n */
inline QString i18n(const char *text)
{ return ki18n(text).toString(); }
template <typename A1>
inline QString i18n(const char *text, const A1 &a1)
{ return ki18n(text).subs(a1).toString(); }
template <typename A1, typename A2>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2)
{ return ki18n(text).subs(a1).subs(a2).toString(); }
template <typename A1, typename A2, typename A3>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).toString(); }
template <typename A1, typename A2, typename A3, typename A4>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline QString i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{ return ki18n(text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).subs(a9).toString(); }

/* i18nc */
inline QString i18nc(const char *ctxt, const char *text)
{ return ki18nc(ctxt, text).toString(); }
template <typename A1>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1)
{ return ki18nc(ctxt, text).subs(a1).toString(); }
template <typename A1, typename A2>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).toString(); }
template <typename A1, typename A2, typename A3>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).toString(); }
template <typename A1, typename A2, typename A3, typename A4>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline QString i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{ return ki18nc(ctxt, text).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).subs(a9).toString(); }

/* i18np */
template <typename A1>
inline QString i18np(const char *sing, const char *plur, const A1 &a1)
{ return ki18np(sing, plur).subs(a1).toString(); }
template <typename A1, typename A2>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2)
{ return ki18np(sing, plur).subs(a1).subs(a2).toString(); }
template <typename A1, typename A2, typename A3>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).toString(); }
template <typename A1, typename A2, typename A3, typename A4>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline QString i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{ return ki18np(sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).subs(a9).toString(); }

/* i18ncp */
template <typename A1>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1)
{ return ki18ncp(ctxt, sing, plur).subs(a1).toString(); }
template <typename A1, typename A2>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).toString(); }
template <typename A1, typename A2, typename A3>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).toString(); }
template <typename A1, typename A2, typename A3, typename A4>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).toString(); }
template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline QString i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6, const A7 &a7, const A8 &a8, const A9 &a9)
{ return ki18ncp(ctxt, sing, plur).subs(a1).subs(a2).subs(a3).subs(a4).subs(a5).subs(a6).subs(a7).subs(a8).subs(a9).toString(); }

#endif
