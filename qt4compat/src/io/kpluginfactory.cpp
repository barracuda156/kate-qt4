/* qt4compat: kpluginfactory.cpp */
#include <kpluginfactory.h>

#include <kaboutdata.h>

#include <QtCore/QList>
#include <QtCore/QMetaObject>
#include <QtCore/QPair>

namespace {
struct Registration
{
    QString keyword;
    const QMetaObject *metaObject;
    KPluginFactory::CreateInstanceFunction create;
};
}

class KPluginFactoryPrivate
{
public:
    KComponentData componentData;
    QList<Registration> registrations;
};

KPluginFactory::KPluginFactory(const char *componentName, const char *catalogName, QObject *parent)
    : QObject(parent), d(new KPluginFactoryPrivate)
{
    if (componentName)
        d->componentData = KComponentData(QByteArray(componentName), QByteArray(catalogName ? catalogName : componentName),
                                          KComponentData::SkipMainComponentRegistration);
}

KPluginFactory::KPluginFactory(const KAboutData &aboutData, QObject *parent)
    : QObject(parent), d(new KPluginFactoryPrivate)
{
    d->componentData = KComponentData(aboutData, KComponentData::SkipMainComponentRegistration);
}

KPluginFactory::KPluginFactory(const KAboutData *aboutData, QObject *parent)
    : QObject(parent), d(new KPluginFactoryPrivate)
{
    if (aboutData)
        d->componentData = KComponentData(*aboutData, KComponentData::SkipMainComponentRegistration);
}

KPluginFactory::~KPluginFactory()
{
    delete d;
}

KComponentData KPluginFactory::componentData() const
{
    return d->componentData;
}

void KPluginFactory::setComponentData(const KComponentData &componentData)
{
    d->componentData = componentData;
}

void KPluginFactory::registerPlugin(const QString &keyword, const QMetaObject *metaObject, CreateInstanceFunction instanceFunction)
{
    Registration r;
    r.keyword = keyword;
    r.metaObject = metaObject;
    r.create = instanceFunction;
    d->registrations.append(r);
}

static bool metaObjectInherits(const QMetaObject *mo, const char *iface)
{
    for (; mo; mo = mo->superClass()) {
        if (qstrcmp(mo->className(), iface) == 0)
            return true;
    }
    return false;
}

QObject *KPluginFactory::create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword)
{
    foreach (const Registration &r, d->registrations) {
        if (r.keyword != keyword)
            continue;
        if (iface && iface[0] && !metaObjectInherits(r.metaObject, iface))
            continue;
        if (!r.create)
            continue;
        QObject *o = r.create(parentWidget, parent, args);
        if (o)
            emit objectCreated(o);
        return o;
    }
    return 0;
}

QVariantList KPluginFactory::stringListToVariantList(const QStringList &list)
{
    QVariantList result;
    foreach (const QString &s, list)
        result << QVariant(s);
    return result;
}

QStringList KPluginFactory::variantListToStringList(const QVariantList &list)
{
    QStringList result;
    foreach (const QVariant &v, list)
        result << v.toString();
    return result;
}
