/* qt4compat: klocale.cpp - QLocale-backed KLocale subset */
#include <klocale.h>

#include <QtCore/QTextCodec>

class KLocalePrivate
{
public:
    KLocalePrivate() : locale(QLocale::system()), mib(0) {}
    QLocale locale;
    int mib;
};

KLocale::KLocale(const QString &, const QString &language, const QString &country)
    : d(new KLocalePrivate)
{
    if (!language.isEmpty()) {
        QString name = language;
        if (!country.isEmpty())
            name += QLatin1Char('_') + country;
        d->locale = QLocale(name);
    }
}

KLocale::~KLocale()
{
    delete d;
}

QString KLocale::formatNumber(double num, int precision) const
{
    if (precision < 0)
        precision = 2;
    return d->locale.toString(num, 'f', precision);
}

QString KLocale::formatNumber(const QString &numStr, bool, int precision) const
{
    bool ok = false;
    const double v = numStr.toDouble(&ok);
    if (!ok)
        return numStr;
    return formatNumber(v, precision);
}

QString KLocale::formatLong(long num) const
{
    return d->locale.toString(qlonglong(num));
}

double KLocale::readNumber(const QString &numStr, bool *ok) const
{
    return d->locale.toDouble(numStr, ok);
}

QString KLocale::formatDate(const QDate &date, DateFormat format) const
{
    switch (format) {
    case IsoDate:
        return date.toString(Qt::ISODate);
    case IsoWeekDate: {
        int year = 0;
        const int week = date.weekNumber(&year);
        return QString::fromLatin1("%1-W%2-%3").arg(year, 4, 10, QLatin1Char('0')).arg(week, 2, 10, QLatin1Char('0')).arg(date.dayOfWeek());
    }
    case IsoOrdinalDate:
        return QString::fromLatin1("%1-%2").arg(date.year(), 4, 10, QLatin1Char('0')).arg(date.dayOfYear(), 3, 10, QLatin1Char('0'));
    case ShortDate:
    case FancyShortDate:
        return d->locale.toString(date, QLocale::ShortFormat);
    case LongDate:
    case FancyLongDate:
    default:
        return d->locale.toString(date, QLocale::LongFormat);
    }
}

QString KLocale::formatTime(const QTime &time, bool includeSecs, bool isDuration) const
{
    TimeFormatOptions options = TimeDefault;
    if (!includeSecs)
        options |= TimeWithoutSeconds;
    if (isDuration)
        options |= TimeDuration;
    return formatTime(time, options);
}

QString KLocale::formatTime(const QTime &time, TimeFormatOptions options) const
{
    if (options & TimeDuration) {
        if (options & TimeWithoutSeconds)
            return time.toString(QLatin1String("hh:mm"));
        return time.toString(QLatin1String("hh:mm:ss"));
    }
    QString fmt = d->locale.timeFormat(QLocale::ShortFormat);
    if (!(options & TimeWithoutSeconds) && !fmt.contains(QLatin1String("ss"))) {
        const int pos = fmt.indexOf(QLatin1String("mm"));
        if (pos >= 0)
            fmt.insert(pos + 2, QLatin1String(":ss"));
    }
    if ((options & TimeWithoutSeconds) && fmt.contains(QLatin1String(":ss")))
        fmt.remove(QLatin1String(":ss"));
    return d->locale.toString(time, fmt);
}

QString KLocale::formatDateTime(const QDateTime &dateTime, DateFormat format, bool includeSecs) const
{
    return formatDate(dateTime.date(), format) + QLatin1Char(' ') + formatTime(dateTime.time(), includeSecs, false);
}

QString KLocale::formatDateTime(const QDateTime &dateTime, DateFormat format, DateTimeFormatOptions options) const
{
    return formatDateTime(dateTime, format, bool(options & Seconds));
}

QString KLocale::formatByteSize(double size) const
{
    const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    int unit = 0;
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        ++unit;
    }
    return formatNumber(size, unit == 0 ? 0 : 1) + QLatin1Char(' ') + QLatin1String(units[unit]);
}

QString KLocale::decimalSymbol() const
{
    return QString(d->locale.decimalPoint());
}

QString KLocale::thousandsSeparator() const
{
    return QString(d->locale.groupSeparator());
}

QString KLocale::dateFormat() const
{
    return d->locale.dateFormat(QLocale::LongFormat);
}

QString KLocale::dateFormatShort() const
{
    return d->locale.dateFormat(QLocale::ShortFormat);
}

QString KLocale::timeFormat() const
{
    return d->locale.timeFormat(QLocale::ShortFormat);
}

QTextCodec *KLocale::codecForEncoding() const
{
    if (d->mib)
        return QTextCodec::codecForMib(d->mib);
    return QTextCodec::codecForLocale();
}

const QByteArray KLocale::encoding() const
{
    QTextCodec *c = codecForEncoding();
    return c ? c->name() : QByteArray("UTF-8");
}

int KLocale::encodingMib() const
{
    QTextCodec *c = codecForEncoding();
    return c ? c->mibEnum() : 106;
}

bool KLocale::setEncoding(int mibEnum)
{
    if (!QTextCodec::codecForMib(mibEnum))
        return false;
    d->mib = mibEnum;
    return true;
}

QString KLocale::language() const
{
    return d->locale.name().section(QLatin1Char('_'), 0, 0);
}

QStringList KLocale::languageList() const
{
    return QStringList() << language();
}

QString KLocale::country() const
{
    return d->locale.name().section(QLatin1Char('_'), 1, 1).toLower();
}

QStringList KLocale::allLanguagesList() const
{
    return QStringList() << QLatin1String("en_US");
}

QString KLocale::languageCodeToName(const QString &language) const
{
    return QLocale::languageToString(QLocale(language).language());
}

QString KLocale::countryCodeToName(const QString &country) const
{
    return QLocale::countryToString(QLocale(QLatin1String("en_") + country.toUpper()).country());
}

bool KLocale::isApplicationTranslatedInto(const QString &language)
{
    return language.startsWith(QLatin1String("en"));
}

bool KLocale::insertCatalog(const QString &)
{
    return true;
}

void KLocale::removeCatalog(const QString &)
{
}

void KLocale::setActiveCatalog(const QString &)
{
}

QString KLocale::defaultLanguage()
{
    return QLatin1String("en_US");
}

QString KLocale::defaultCountry()
{
    return QLatin1String("C");
}

void KLocale::splitLocale(const QString &locale, QString &language, QString &country, QString &modifier, QString &charset)
{
    QString l = locale;
    charset.clear();
    modifier.clear();
    country.clear();
    int pos = l.indexOf(QLatin1Char('.'));
    if (pos >= 0) {
        charset = l.mid(pos + 1);
        l.truncate(pos);
    }
    pos = l.indexOf(QLatin1Char('@'));
    if (pos >= 0) {
        modifier = l.mid(pos + 1);
        l.truncate(pos);
    }
    pos = l.indexOf(QLatin1Char('_'));
    if (pos >= 0) {
        country = l.mid(pos + 1);
        l.truncate(pos);
    }
    language = l;
}

QString KLocale::translateQt(const char *, const char *sourceText, const char *) const
{
    return QString::fromUtf8(sourceText);
}
