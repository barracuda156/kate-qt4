/* qt4compat: kparts/factory.h - KParts::Factory without KLibFactory (parts are linked in) */
#ifndef KDECOMPAT_KPARTS_FACTORY_H
#define KDECOMPAT_KPARTS_FACTORY_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <kcomponentdata.h>
#include <kparts/kparts_export.h>

class QWidget;

namespace KParts
{

class Part;

class KPARTS_EXPORT Factory : public QObject
{
    Q_OBJECT
public:
    Factory(QObject *parent = 0);
    virtual ~Factory();

    Part *createPart(QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KParts::Part",
                     const QStringList &args = QStringList());
    virtual KComponentData partComponentData();
    static KComponentData partComponentDataFromLibrary(const QString &libraryName);
    QObject *create(QObject *parent = 0, const char *classname = "QObject", const QStringList &args = QStringList());

Q_SIGNALS:
    void objectCreated(QObject *object);

protected:
    virtual Part *createPartObject(QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KParts::Part",
                                   const QStringList &args = QStringList()) = 0;
    virtual QObject *createObject(QObject *parent = 0, const char *classname = "QObject", const QStringList &args = QStringList());
};

}

#endif
