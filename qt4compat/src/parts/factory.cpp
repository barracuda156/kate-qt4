/* qt4compat: factory.cpp */
#include <kparts/factory.h>
#include <kparts/part.h>

#include <QtGui/QWidget>

using namespace KParts;

Factory::Factory(QObject *parent)
    : QObject(parent)
{
}

Factory::~Factory()
{
}

Part *Factory::createPart(QWidget *parentWidget, QObject *parent, const char *classname, const QStringList &args)
{
    Part *part = createPartObject(parentWidget, parent, classname, args);
    if (part)
        emit objectCreated(part);
    return part;
}

KComponentData Factory::partComponentData()
{
    return KComponentData();
}

KComponentData Factory::partComponentDataFromLibrary(const QString &)
{
    return KComponentData();
}

QObject *Factory::create(QObject *parent, const char *classname, const QStringList &args)
{
    return createObject(parent, classname, args);
}

QObject *Factory::createObject(QObject *parent, const char *classname, const QStringList &args)
{
    QWidget *parentWidget = parent && parent->isWidgetType() ? static_cast<QWidget *>(parent) : 0;
    return createPart(parentWidget, parent, classname, args);
}
