/* qt4compat: klocale.h - the parts of KLocale Kate uses, on top of QLocale. */
#ifndef KDECOMPAT_KLOCALE_H
#define KDECOMPAT_KLOCALE_H

#include <QtCore/QDateTime>
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <klocalizedstring.h>

class QTextCodec;
class KLocalePrivate;

class KDECORE_EXPORT KLocale
{
public:
    enum DateFormat {
        ShortDate,
        LongDate,
        FancyShortDate,
        FancyLongDate,
        IsoDate,
        IsoWeekDate,
        IsoOrdinalDate
    };
    enum TimeFormatOption {
        TimeDefault = 0x0,
        TimeDuration = 0x1,
        TimeWithoutAmPm = 0x2,
        TimeWithoutSeconds = 0x4,
        TimeFoldHours = 0x8
    };
    Q_DECLARE_FLAGS(TimeFormatOptions, TimeFormatOption)
    enum DateTimeFormatOption {
        TimeZone = 0x01,
        Seconds = 0x02
    };
    Q_DECLARE_FLAGS(DateTimeFormatOptions, DateTimeFormatOption)
    enum MeasureSystem { Metric, Imperial };

    explicit KLocale(const QString &catalog = QString(), const QString &language = QString(), const QString &country = QString());
    ~KLocale();

    QString formatNumber(double num, int precision = -1) const;
    QString formatNumber(const QString &numStr, bool round = true, int precision = -1) const;
    QString formatLong(long num) const;
    double readNumber(const QString &numStr, bool *ok = 0) const;

    QString formatDate(const QDate &date, DateFormat format = LongDate) const;
    QString formatTime(const QTime &time, bool includeSecs = false, bool isDuration = false) const;
    QString formatTime(const QTime &time, TimeFormatOptions options) const;
    QString formatDateTime(const QDateTime &dateTime, DateFormat format = ShortDate, bool includeSecs = false) const;
    QString formatDateTime(const QDateTime &dateTime, DateFormat format, DateTimeFormatOptions options) const;
    QString formatByteSize(double size) const;

    QString decimalSymbol() const;
    QString thousandsSeparator() const;
    QString dateFormat() const;
    QString dateFormatShort() const;
    QString timeFormat() const;

    QTextCodec *codecForEncoding() const;
    const QByteArray encoding() const;
    int encodingMib() const;
    bool setEncoding(int mibEnum);

    QString language() const;
    QStringList languageList() const;
    QString country() const;
    QStringList allLanguagesList() const;
    QString languageCodeToName(const QString &language) const;
    QString countryCodeToName(const QString &country) const;
    bool isApplicationTranslatedInto(const QString &language);

    bool insertCatalog(const QString &catalog);
    void removeCatalog(const QString &catalog);
    void setActiveCatalog(const QString &catalog);

    static QString defaultLanguage();
    static QString defaultCountry();
    static void splitLocale(const QString &locale, QString &language, QString &country, QString &modifier, QString &charset);

    /* translate(): identity, kept for API completeness */
    QString translateQt(const char *context, const char *sourceText, const char *comment) const;

private:
    KLocalePrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KLocale::TimeFormatOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KLocale::DateTimeFormatOptions)

#endif
