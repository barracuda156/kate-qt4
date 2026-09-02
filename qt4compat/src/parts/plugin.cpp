/* qt4compat: plugin.cpp - KParts::Plugin stub */
#include <kparts/plugin.h>
#include <kcomponentdata.h>

using namespace KParts;

Plugin::Plugin(QObject *parent)
    : QObject(parent)
{
}

Plugin::~Plugin()
{
}

QString Plugin::xmlFile() const
{
    return KXMLGUIClient::xmlFile();
}

QString Plugin::localXMLFile() const
{
    return KXMLGUIClient::localXMLFile();
}

void Plugin::loadPlugins(QObject *, const KComponentData &)
{
}

void Plugin::loadPlugins(QObject *, const QList<PluginInfo> &)
{
}

void Plugin::loadPlugins(QObject *, const QList<PluginInfo> &, const KComponentData &)
{
}

void Plugin::loadPlugins(QObject *, KXMLGUIClient *, const KComponentData &, bool, int)
{
}

QList<Plugin *> Plugin::pluginObjects(QObject *parent)
{
    QList<Plugin *> objects;
    if (!parent)
        return objects;
    foreach (QObject *child, parent->children()) {
        if (Plugin *p = qobject_cast<Plugin *>(child))
            objects.append(p);
    }
    return objects;
}

QList<Plugin::PluginInfo> Plugin::pluginInfos(const KComponentData &)
{
    return QList<Plugin::PluginInfo>();
}

Plugin *Plugin::loadPlugin(QObject *, const char *)
{
    return 0;
}

Plugin *Plugin::loadPlugin(QObject *, const QString &)
{
    return 0;
}

void Plugin::setComponentData(const KComponentData &instance)
{
    KXMLGUIClient::setComponentData(instance);
}
