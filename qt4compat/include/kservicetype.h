/* qt4compat: kservicetype.h - stub */
#ifndef KDECOMPAT_KSERVICETYPE_H
#define KDECOMPAT_KSERVICETYPE_H

#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <kdemacros.h>
#include <ksharedptr.h>

class KDECORE_EXPORT KServiceType : public QSharedData
{
public:
    typedef KSharedPtr<KServiceType> Ptr;
    typedef QList<Ptr> List;

    virtual ~KServiceType() {}
    QString name() const { return QString(); }
    QString comment() const { return QString(); }
    bool isDerived() const { return false; }
    QString parentServiceType() const { return QString(); }
    bool inherits(const QString &) const { return false; }
    QVariant::Type propertyDef(const QString &) const { return QVariant::Invalid; }
    QVariant property(const QString &) const { return QVariant(); }

    static Ptr serviceType(const QString &) { return Ptr(); }
    static List allServiceTypes() { return List(); }
};

#endif
