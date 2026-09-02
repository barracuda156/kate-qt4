/* qt4compat: kcomponentdata.cpp */
#include <kcomponentdata.h>
#include "kglobal_p.h"

#include <kaboutdata.h>
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kstandarddirs.h>

#include <QtCore/QAtomicInt>

class KComponentDataPrivate
{
public:
    KComponentDataPrivate(const QByteArray &n, const QByteArray &c)
        : refCount(1), name(n), catalog(c), about(0), dirs(0) {}
    ~KComponentDataPrivate()
    {
        delete about;
        delete dirs;
    }
    void ref() { refCount.ref(); }
    void deref()
    {
        if (!refCount.deref())
            delete this;
    }

    QAtomicInt refCount;
    QByteArray name;
    QByteArray catalog;
    KAboutData *about;
    KSharedConfigPtr config;
    QString configName;
    KStandardDirs *dirs;
};

KComponentData::KComponentData()
    : d(0)
{
}

KComponentData::KComponentData(const KComponentData &rhs)
    : d(rhs.d)
{
    if (d)
        d->ref();
}

KComponentData &KComponentData::operator=(const KComponentData &rhs)
{
    if (rhs.d != d) {
        if (rhs.d)
            rhs.d->ref();
        if (d)
            d->deref();
        d = rhs.d;
    }
    return *this;
}

bool KComponentData::operator==(const KComponentData &rhs) const
{
    return d == rhs.d;
}

KComponentData::KComponentData(const QByteArray &componentName, const QByteArray &catalogName, MandatoryFlags registerAsMain)
    : d(new KComponentDataPrivate(componentName, catalogName.isEmpty() ? componentName : catalogName))
{
    if (registerAsMain == RegisterAsMainComponent && !KGlobal::hasMainComponent())
        KGlobal::setMainComponent(*this);
}

KComponentData::KComponentData(const KAboutData *aboutData, MandatoryFlags registerAsMain)
    : d(new KComponentDataPrivate(aboutData->appName().toUtf8(), aboutData->catalogName().toUtf8()))
{
    d->about = new KAboutData(*aboutData);
    if (registerAsMain == RegisterAsMainComponent && !KGlobal::hasMainComponent())
        KGlobal::setMainComponent(*this);
}

KComponentData::KComponentData(const KAboutData &aboutData, MandatoryFlags registerAsMain)
    : d(new KComponentDataPrivate(aboutData.appName().toUtf8(), aboutData.catalogName().toUtf8()))
{
    d->about = new KAboutData(aboutData);
    if (registerAsMain == RegisterAsMainComponent && !KGlobal::hasMainComponent())
        KGlobal::setMainComponent(*this);
}

KComponentData::~KComponentData()
{
    if (d)
        d->deref();
}

bool KComponentData::isValid() const
{
    return d != 0;
}

KStandardDirs *KComponentData::dirs() const
{
    if (!d)
        return KGlobal::dirs();
    if (!d->dirs) {
        d->dirs = new KStandardDirs;
        d->dirs->setComponentName(QString::fromUtf8(d->name));
    }
    return d->dirs;
}

const KSharedConfigPtr &KComponentData::config() const
{
    static KSharedConfigPtr invalid;
    if (!d)
        return invalid;
    if (!d->config) {
        QString name = d->configName;
        if (name.isEmpty())
            name = QString::fromUtf8(d->name) + QLatin1String("rc");
        d->config = KSharedConfig::openConfig(*this, name);
    }
    return d->config;
}

void KComponentData::setConfigName(const QString &name)
{
    if (d) {
        d->configName = name;
        d->config = KSharedConfigPtr();
    }
}

const KAboutData *KComponentData::aboutData() const
{
    if (!d)
        return 0;
    if (!d->about)
        d->about = new KAboutData(d->name, d->catalog, ki18n(d->name.constData()), QByteArray("unknown"));
    return d->about;
}

void KComponentData::setAboutData(const KAboutData &aboutData)
{
    if (d) {
        delete d->about;
        d->about = new KAboutData(aboutData);
    }
}

QString KComponentData::componentName() const
{
    return d ? QString::fromUtf8(d->name) : QString();
}

QString KComponentData::catalogName() const
{
    return d ? QString::fromUtf8(d->catalog) : QString();
}

void KComponentData::virtual_hook(int, void *)
{
}
