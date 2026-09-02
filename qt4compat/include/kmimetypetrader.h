/* qt4compat: kmimetypetrader.h - no service database, all queries are empty */
#ifndef KDECOMPAT_KMIMETYPETRADER_H
#define KDECOMPAT_KMIMETYPETRADER_H

#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <kdemacros.h>
#include <kservice.h>

class KDECORE_EXPORT KMimeTypeTrader
{
public:
    static KMimeTypeTrader *self();

    KService::List query(const QString &mimeType, const QString &genericServiceType = QLatin1String("Application"),
                         const QString &constraint = QString()) const;
    KService::Ptr preferredService(const QString &mimeType, const QString &genericServiceType = QLatin1String("Application"));

    template <class T>
    T *createPartInstanceFromQuery(const QString &mimeType, QWidget *parentWidget = 0, QObject *parent = 0,
                                   const QString &constraint = QString(), const QVariantList &args = QVariantList(),
                                   QString *error = 0) const
    {
        Q_UNUSED(mimeType)
        Q_UNUSED(parentWidget)
        Q_UNUSED(parent)
        Q_UNUSED(constraint)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("no services are available");
        return 0;
    }
    template <class T>
    T *createInstanceFromQuery(const QString &mimeType, const QString &serviceType, QObject *parent = 0,
                               const QString &constraint = QString(), const QVariantList &args = QVariantList(),
                               QString *error = 0) const
    {
        Q_UNUSED(mimeType)
        Q_UNUSED(serviceType)
        Q_UNUSED(parent)
        Q_UNUSED(constraint)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("no services are available");
        return 0;
    }

private:
    KMimeTypeTrader();
};

#endif
