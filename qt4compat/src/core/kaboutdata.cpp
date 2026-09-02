/* qt4compat: kaboutdata.cpp */
#include <kaboutdata.h>

#include <QtCore/QFile>

class KAboutPersonPrivate
{
public:
    QString name, task, email, web, ocs;
};

KAboutPerson::KAboutPerson(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress)
    : d(new KAboutPersonPrivate)
{
    d->name = name.toString();
    d->task = task.toString();
    d->email = QString::fromUtf8(emailAddress);
    d->web = QString::fromUtf8(webAddress);
}

KAboutPerson::KAboutPerson(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername)
    : d(new KAboutPersonPrivate)
{
    d->name = name.toString();
    d->task = task.toString();
    d->email = QString::fromUtf8(emailAddress);
    d->web = QString::fromUtf8(webAddress);
    d->ocs = QString::fromUtf8(ocsUsername);
}

KAboutPerson::KAboutPerson(const QString &name, const QString &email)
    : d(new KAboutPersonPrivate)
{
    d->name = name;
    d->email = email;
}

KAboutPerson::KAboutPerson(const KAboutPerson &other)
    : d(new KAboutPersonPrivate(*other.d))
{
}

KAboutPerson::~KAboutPerson()
{
    delete d;
}

KAboutPerson &KAboutPerson::operator=(const KAboutPerson &other)
{
    if (this != &other)
        *d = *other.d;
    return *this;
}

QString KAboutPerson::name() const { return d->name; }
QString KAboutPerson::task() const { return d->task; }
QString KAboutPerson::emailAddress() const { return d->email; }
QString KAboutPerson::webAddress() const { return d->web; }
QString KAboutPerson::ocsUsername() const { return d->ocs; }

class KAboutLicensePrivate
{
public:
    int key;
    QString text;
};

KAboutLicense::KAboutLicense(int licenseType, const KAboutData *)
    : d(new KAboutLicensePrivate)
{
    d->key = licenseType;
}

KAboutLicense::KAboutLicense(const KAboutLicense &other)
    : d(new KAboutLicensePrivate(*other.d))
{
}

KAboutLicense::~KAboutLicense()
{
    delete d;
}

KAboutLicense &KAboutLicense::operator=(const KAboutLicense &other)
{
    if (this != &other)
        *d = *other.d;
    return *this;
}

int KAboutLicense::key() const
{
    return d->key;
}

QString KAboutLicense::name(NameFormat formatName) const
{
    const bool full = formatName == FullName;
    switch (d->key) {
    case KAboutData::License_GPL_V2: return full ? i18n("GNU General Public License Version 2") : QLatin1String("GPL v2");
    case KAboutData::License_LGPL_V2: return full ? i18n("GNU Lesser General Public License Version 2") : QLatin1String("LGPL v2");
    case KAboutData::License_BSD: return full ? i18n("BSD License") : QLatin1String("BSD License");
    case KAboutData::License_Artistic: return full ? i18n("Artistic License") : QLatin1String("Artistic License");
    case KAboutData::License_QPL_V1_0: return full ? i18n("Q Public License") : QLatin1String("QPL v1.0");
    case KAboutData::License_GPL_V3: return full ? i18n("GNU General Public License Version 3") : QLatin1String("GPL v3");
    case KAboutData::License_LGPL_V3: return full ? i18n("GNU Lesser General Public License Version 3") : QLatin1String("LGPL v3");
    case KAboutData::License_Custom:
    case KAboutData::License_File: return i18n("Custom");
    default: return i18n("Not specified");
    }
}

QString KAboutLicense::text() const
{
    if (!d->text.isEmpty())
        return d->text;
    return name(FullName);
}

class KAboutDataPrivate
{
public:
    QByteArray appName, catalogName, version, homepage, bugAddress, organizationDomain, productName;
    KLocalizedString programName, shortDescription, copyright, otherText, customPlain, customRich;
    KAboutData::LicenseKey license;
    QString licenseText, programIconName;
    QVariant programLogo;
    QList<KAboutPerson> authors, credits, translators;
    bool customAuthorText;
};

KAboutData::KAboutData(const QByteArray &appName, const QByteArray &catalogName, const KLocalizedString &programName,
                       const QByteArray &version, const KLocalizedString &shortDescription, enum LicenseKey licenseType,
                       const KLocalizedString &copyrightStatement, const KLocalizedString &otherText,
                       const QByteArray &homePageAddress, const QByteArray &bugsEmailAddress)
    : d(new KAboutDataPrivate)
{
    d->appName = appName;
    const int slash = d->appName.lastIndexOf('/');
    if (slash >= 0)
        d->appName = d->appName.mid(slash + 1);
    d->catalogName = catalogName.isEmpty() ? d->appName : catalogName;
    d->programName = programName;
    d->version = version;
    d->shortDescription = shortDescription;
    d->license = licenseType;
    d->copyright = copyrightStatement;
    d->otherText = otherText;
    d->homepage = homePageAddress;
    d->bugAddress = bugsEmailAddress;
    d->customAuthorText = false;
    d->organizationDomain = "kde.org";
    if (!homePageAddress.isEmpty()) {
        QString host = QString::fromUtf8(homePageAddress);
        const int idx = host.indexOf(QLatin1String("://"));
        if (idx >= 0)
            host = host.mid(idx + 3);
        host = host.section(QLatin1Char('/'), 0, 0);
        if (host.startsWith(QLatin1String("www.")))
            host = host.mid(4);
        if (!host.isEmpty())
            d->organizationDomain = host.toUtf8();
    }
}

KAboutData::KAboutData(const KAboutData &other)
    : d(new KAboutDataPrivate(*other.d))
{
}

KAboutData &KAboutData::operator=(const KAboutData &other)
{
    if (this != &other)
        *d = *other.d;
    return *this;
}

KAboutData::~KAboutData()
{
    delete d;
}

KAboutData &KAboutData::addAuthor(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress)
{
    d->authors.append(KAboutPerson(name, task, emailAddress, webAddress));
    return *this;
}

KAboutData &KAboutData::addAuthor(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername)
{
    d->authors.append(KAboutPerson(name, task, emailAddress, webAddress, ocsUsername));
    return *this;
}

KAboutData &KAboutData::addCredit(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress)
{
    d->credits.append(KAboutPerson(name, task, emailAddress, webAddress));
    return *this;
}

KAboutData &KAboutData::addCredit(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername)
{
    d->credits.append(KAboutPerson(name, task, emailAddress, webAddress, ocsUsername));
    return *this;
}

KAboutData &KAboutData::setTranslator(const KLocalizedString &name, const KLocalizedString &emailAddress)
{
    d->translators.clear();
    if (!name.isEmpty())
        d->translators.append(KAboutPerson(name.toString(), emailAddress.toString()));
    return *this;
}

KAboutData &KAboutData::setLicenseText(const KLocalizedString &license)
{
    d->license = License_Custom;
    d->licenseText = license.toString();
    return *this;
}

KAboutData &KAboutData::addLicenseText(const KLocalizedString &license)
{
    return setLicenseText(license);
}

KAboutData &KAboutData::setLicenseTextFile(const QString &file)
{
    d->license = License_File;
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
        d->licenseText = QString::fromUtf8(f.readAll());
    return *this;
}

KAboutData &KAboutData::addLicenseTextFile(const QString &file)
{
    return setLicenseTextFile(file);
}

KAboutData &KAboutData::setAppName(const QByteArray &appName) { d->appName = appName; return *this; }
KAboutData &KAboutData::setProgramName(const KLocalizedString &programName) { d->programName = programName; return *this; }
KAboutData &KAboutData::setProgramIconName(const QString &iconName) { d->programIconName = iconName; return *this; }
KAboutData &KAboutData::setProgramLogo(const QVariant &image) { d->programLogo = image; return *this; }
KAboutData &KAboutData::setOcsProvider(const QByteArray &) { return *this; }
KAboutData &KAboutData::setVersion(const QByteArray &version) { d->version = version; return *this; }
KAboutData &KAboutData::setShortDescription(const KLocalizedString &shortDescription) { d->shortDescription = shortDescription; return *this; }
KAboutData &KAboutData::setCatalogName(const QByteArray &catalogName) { d->catalogName = catalogName; return *this; }
KAboutData &KAboutData::setLicense(LicenseKey licenseKey) { d->license = licenseKey; return *this; }
KAboutData &KAboutData::addLicense(LicenseKey licenseKey) { d->license = licenseKey; return *this; }
KAboutData &KAboutData::setCopyrightStatement(const KLocalizedString &copyrightStatement) { d->copyright = copyrightStatement; return *this; }
KAboutData &KAboutData::setOtherText(const KLocalizedString &otherText) { d->otherText = otherText; return *this; }
KAboutData &KAboutData::setHomepage(const QByteArray &homepage) { d->homepage = homepage; return *this; }
KAboutData &KAboutData::setBugAddress(const QByteArray &bugAddress) { d->bugAddress = bugAddress; return *this; }
KAboutData &KAboutData::setOrganizationDomain(const QByteArray &domain) { d->organizationDomain = domain; return *this; }
KAboutData &KAboutData::setProductName(const QByteArray &name) { d->productName = name; return *this; }

KAboutData &KAboutData::setCustomAuthorText(const KLocalizedString &plainText, const KLocalizedString &richText)
{
    d->customPlain = plainText;
    d->customRich = richText;
    d->customAuthorText = true;
    return *this;
}

KAboutData &KAboutData::unsetCustomAuthorText()
{
    d->customAuthorText = false;
    return *this;
}

QString KAboutData::appName() const { return QString::fromUtf8(d->appName); }
QString KAboutData::productName() const { return d->productName.isEmpty() ? appName() : QString::fromUtf8(d->productName); }
QString KAboutData::programName() const { return d->programName.isEmpty() ? appName() : d->programName.toString(); }
QString KAboutData::organizationDomain() const { return QString::fromUtf8(d->organizationDomain); }
const char *KAboutData::internalProgramName() const { return d->appName.constData(); }
void KAboutData::translateInternalProgramName() const {}
QString KAboutData::programIconName() const { return d->programIconName.isEmpty() ? appName() : d->programIconName; }
QVariant KAboutData::programLogo() const { return d->programLogo; }
QString KAboutData::ocsProviderUrl() const { return QString(); }
QString KAboutData::version() const { return QString::fromUtf8(d->version); }
const char *KAboutData::internalVersion() const { return d->version.constData(); }
QString KAboutData::shortDescription() const { return d->shortDescription.toString(); }
QString KAboutData::catalogName() const { return QString::fromUtf8(d->catalogName); }
QString KAboutData::homepage() const { return QString::fromUtf8(d->homepage); }
QString KAboutData::bugAddress() const { return QString::fromUtf8(d->bugAddress); }
const char *KAboutData::internalBugAddress() const { return d->bugAddress.constData(); }
QList<KAboutPerson> KAboutData::authors() const { return d->authors; }
QList<KAboutPerson> KAboutData::credits() const { return d->credits; }
QList<KAboutPerson> KAboutData::translators() const { return d->translators; }
QString KAboutData::aboutTranslationTeam() { return QString(); }
QString KAboutData::otherText() const { return d->otherText.toString(); }

QString KAboutData::license() const
{
    return licenseInfo().text();
}

QString KAboutData::licenseName(NameFormat nameFormat) const
{
    return licenseInfo().name(nameFormat == FullName ? KAboutLicense::FullName : KAboutLicense::ShortName);
}

KAboutLicense KAboutData::licenseInfo() const
{
    KAboutLicense l(d->license, this);
    l.d->text = d->licenseText;
    return l;
}

QList<KAboutLicense> KAboutData::licenses() const
{
    return QList<KAboutLicense>() << licenseInfo();
}

QString KAboutData::copyrightStatement() const { return d->copyright.toString(); }
QString KAboutData::customAuthorPlainText() const { return d->customPlain.toString(); }
QString KAboutData::customAuthorRichText() const { return d->customRich.toString(); }
bool KAboutData::customAuthorTextEnabled() const { return d->customAuthorText; }
