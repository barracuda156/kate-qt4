/* qt4compat: kparts/plugin.h - KParts::Plugin stub; there are no loadable KParts plugins */
#ifndef KDECOMPAT_KPARTS_PLUGIN_H
#define KDECOMPAT_KPARTS_PLUGIN_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtXml/QDomDocument>
#include <kaction.h>
#include <kparts/kparts_export.h>
#include <kxmlguiclient.h>

class KComponentData;

namespace KParts
{

class KPARTS_EXPORT Plugin : public QObject, virtual public KXMLGUIClient
{
    Q_OBJECT
public:
    struct PluginInfo
    {
        QString m_relXMLFile;
        QString m_absXMLFile;
        QDomDocument m_document;
    };

    explicit Plugin(QObject *parent = 0);
    virtual ~Plugin();

    virtual QString xmlFile() const;
    virtual QString localXMLFile() const;

    static void loadPlugins(QObject *parent, const KComponentData &instance);
    static void loadPlugins(QObject *parent, const QList<PluginInfo> &pluginInfos);
    static void loadPlugins(QObject *parent, const QList<PluginInfo> &pluginInfos, const KComponentData &instance);
    static void loadPlugins(QObject *parent, KXMLGUIClient *parentGUIClient, const KComponentData &instance,
                            bool enableNewPluginsByDefault = true, int interfaceVersionRequired = 0);
    static QList<Plugin *> pluginObjects(QObject *parent);

protected:
    static QList<Plugin::PluginInfo> pluginInfos(const KComponentData &instance);
    static Plugin *loadPlugin(QObject *parent, const char *libname);
    static Plugin *loadPlugin(QObject *parent, const QString &libname);
    virtual void setComponentData(const KComponentData &instance);
};

}

#endif
