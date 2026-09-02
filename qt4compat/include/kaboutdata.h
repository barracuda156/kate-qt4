/* qt4compat: kaboutdata.h */
#ifndef KDECOMPAT_KABOUTDATA_H
#define KDECOMPAT_KABOUTDATA_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariant>
#include <kdemacros.h>
#include <klocalizedstring.h>

class KAboutData;
class KAboutPersonPrivate;
class KAboutLicensePrivate;
class KAboutDataPrivate;

class KDECORE_EXPORT KAboutPerson
{
    friend class KAboutData;
public:
    explicit KAboutPerson(const KLocalizedString &name, const KLocalizedString &task = KLocalizedString(),
                          const QByteArray &emailAddress = QByteArray(), const QByteArray &webAddress = QByteArray());
    explicit KAboutPerson(const KLocalizedString &name, const KLocalizedString &task,
                          const QByteArray &emailAddress, const QByteArray &webAddress, const QByteArray &ocsUsername);
    KAboutPerson(const KAboutPerson &other);
    ~KAboutPerson();
    KAboutPerson &operator=(const KAboutPerson &other);

    QString name() const;
    QString task() const;
    QString emailAddress() const;
    QString webAddress() const;
    QString ocsUsername() const;

private:
    KAboutPerson(const QString &name, const QString &email);
    KAboutPersonPrivate *const d;
};

class KDECORE_EXPORT KAboutLicense
{
    friend class KAboutData;
public:
    enum NameFormat { ShortName, FullName };

    KAboutLicense(const KAboutLicense &other);
    ~KAboutLicense();
    KAboutLicense &operator=(const KAboutLicense &other);

    QString text() const;
    QString name(NameFormat formatName) const;
    int key() const;

private:
    KAboutLicense(int licenseType, const KAboutData *aboutData);
    KAboutLicensePrivate *const d;
};

class KDECORE_EXPORT KAboutData
{
public:
    enum LicenseKey {
        License_Custom = -2,
        License_File = -1,
        License_Unknown = 0,
        License_GPL = 1,
        License_GPL_V2 = 1,
        License_LGPL = 2,
        License_LGPL_V2 = 2,
        License_BSD = 3,
        License_Artistic = 4,
        License_QPL = 5,
        License_QPL_V1_0 = 5,
        License_GPL_V3 = 6,
        License_LGPL_V3 = 7
    };
    enum NameFormat { ShortName, FullName };

    KAboutData(const QByteArray &appName, const QByteArray &catalogName, const KLocalizedString &programName,
               const QByteArray &version, const KLocalizedString &shortDescription = KLocalizedString(),
               enum LicenseKey licenseType = License_Unknown,
               const KLocalizedString &copyrightStatement = KLocalizedString(),
               const KLocalizedString &otherText = KLocalizedString(),
               const QByteArray &homePageAddress = QByteArray(),
               const QByteArray &bugsEmailAddress = "submit@bugs.kde.org");
    KAboutData(const KAboutData &other);
    KAboutData &operator=(const KAboutData &other);
    ~KAboutData();

    KAboutData &addAuthor(const KLocalizedString &name, const KLocalizedString &task = KLocalizedString(),
                          const QByteArray &emailAddress = QByteArray(), const QByteArray &webAddress = QByteArray());
    KAboutData &addAuthor(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress,
                          const QByteArray &webAddress, const QByteArray &ocsUsername);
    KAboutData &addCredit(const KLocalizedString &name, const KLocalizedString &task = KLocalizedString(),
                          const QByteArray &emailAddress = QByteArray(), const QByteArray &webAddress = QByteArray());
    KAboutData &addCredit(const KLocalizedString &name, const KLocalizedString &task, const QByteArray &emailAddress,
                          const QByteArray &webAddress, const QByteArray &ocsUsername);
    KAboutData &setTranslator(const KLocalizedString &name, const KLocalizedString &emailAddress);
    KAboutData &setLicenseText(const KLocalizedString &license);
    KAboutData &addLicenseText(const KLocalizedString &license);
    KAboutData &setLicenseTextFile(const QString &file);
    KAboutData &addLicenseTextFile(const QString &file);
    KAboutData &setAppName(const QByteArray &appName);
    KAboutData &setProgramName(const KLocalizedString &programName);
    KAboutData &setProgramIconName(const QString &iconName);
    KAboutData &setProgramLogo(const QVariant &image);
    KAboutData &setOcsProvider(const QByteArray &providerUrl);
    KAboutData &setVersion(const QByteArray &version);
    KAboutData &setShortDescription(const KLocalizedString &shortDescription);
    KAboutData &setCatalogName(const QByteArray &catalogName);
    KAboutData &setLicense(LicenseKey licenseKey);
    KAboutData &addLicense(LicenseKey licenseKey);
    KAboutData &setCopyrightStatement(const KLocalizedString &copyrightStatement);
    KAboutData &setOtherText(const KLocalizedString &otherText);
    KAboutData &setHomepage(const QByteArray &homepage);
    KAboutData &setBugAddress(const QByteArray &bugAddress);
    KAboutData &setOrganizationDomain(const QByteArray &domain);
    KAboutData &setProductName(const QByteArray &name);
    KAboutData &setCustomAuthorText(const KLocalizedString &plainText, const KLocalizedString &richText);
    KAboutData &unsetCustomAuthorText();

    QString appName() const;
    QString productName() const;
    QString programName() const;
    QString organizationDomain() const;
    const char *internalProgramName() const;
    void translateInternalProgramName() const;
    QString programIconName() const;
    QVariant programLogo() const;
    QString ocsProviderUrl() const;
    QString version() const;
    const char *internalVersion() const;
    QString shortDescription() const;
    QString catalogName() const;
    QString homepage() const;
    QString bugAddress() const;
    const char *internalBugAddress() const;
    QList<KAboutPerson> authors() const;
    QList<KAboutPerson> credits() const;
    QList<KAboutPerson> translators() const;
    static QString aboutTranslationTeam();
    QString otherText() const;
    QString license() const;
    QString licenseName(NameFormat nameFormat) const;
    KAboutLicense licenseInfo() const;
    QList<KAboutLicense> licenses() const;
    QString copyrightStatement() const;
    QString customAuthorPlainText() const;
    QString customAuthorRichText() const;
    bool customAuthorTextEnabled() const;

private:
    KAboutDataPrivate *const d;
};

#endif
