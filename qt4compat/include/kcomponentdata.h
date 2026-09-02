/* qt4compat: kcomponentdata.h */
#ifndef KDECOMPAT_KCOMPONENTDATA_H
#define KDECOMPAT_KCOMPONENTDATA_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <kdemacros.h>
#include <ksharedptr.h>

class KAboutData;
class KStandardDirs;
class KSharedConfig;
class KComponentDataPrivate;
typedef KSharedPtr<KSharedConfig> KSharedConfigPtr;

class KDECORE_EXPORT KComponentData
{
public:
    enum MandatoryFlags {
        RegisterAsMainComponent = 1,
        SkipMainComponentRegistration = 2
    };

    KComponentData();
    KComponentData(const KComponentData &rhs);
    KComponentData &operator=(const KComponentData &rhs);
    bool operator==(const KComponentData &rhs) const;
    bool operator!=(const KComponentData &rhs) const { return !operator==(rhs); }

    explicit KComponentData(const QByteArray &componentName, const QByteArray &catalogName = QByteArray(),
                            MandatoryFlags registerAsMain = RegisterAsMainComponent);
    explicit KComponentData(const KAboutData *aboutData, MandatoryFlags registerAsMain = RegisterAsMainComponent);
    explicit KComponentData(const KAboutData &aboutData, MandatoryFlags registerAsMain = RegisterAsMainComponent);
    virtual ~KComponentData();

    bool isValid() const;
    KStandardDirs *dirs() const;
    const KSharedConfigPtr &config() const;
    void setConfigName(const QString &name);
    const KAboutData *aboutData() const;
    void setAboutData(const KAboutData &aboutData);
    QString componentName() const;
    QString catalogName() const;

protected:
    virtual void virtual_hook(int id, void *data);

private:
    KComponentDataPrivate *d;
};

#endif
