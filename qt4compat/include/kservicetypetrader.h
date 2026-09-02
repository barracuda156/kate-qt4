/* qt4compat: kservicetypetrader.h - no service database, all queries are empty */
#ifndef KDECOMPAT_KSERVICETYPETRADER_H
#define KDECOMPAT_KSERVICETYPETRADER_H

#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <kdemacros.h>
#include <kservice.h>

class KDECORE_EXPORT KServiceTypeTrader
{
public:
    static KServiceTypeTrader *self();

    KService::List query(const QString &servicetype, const QString &constraint = QString()) const;
    KService::Ptr preferredService(const QString &serviceType) const;

    template <class T>
    T *createInstanceFromQuery(const QString &serviceType, const QString &constraint = QString(),
                               QObject *parent = 0, const QVariantList &args = QVariantList(), QString *error = 0) const
    {
        Q_UNUSED(serviceType)
        Q_UNUSED(constraint)
        Q_UNUSED(parent)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("no services are available");
        return 0;
    }
    template <class T>
    T *createInstanceFromQuery(const QString &serviceType, QWidget *parentWidget, QObject *parent,
                               const QString &constraint = QString(), const QVariantList &args = QVariantList(),
                               QString *error = 0) const
    {
        Q_UNUSED(serviceType)
        Q_UNUSED(parentWidget)
        Q_UNUSED(parent)
        Q_UNUSED(constraint)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("no services are available");
        return 0;
    }

private:
    KServiceTypeTrader();
};

#endif
