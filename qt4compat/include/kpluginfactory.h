/* qt4compat: kpluginfactory.h - in-process KPluginFactory. registerPlugin<T>()
 * records a creator, create<T>() instantiates it; nothing is loaded from disk. */
#ifndef KDECOMPAT_KPLUGINFACTORY_H
#define KDECOMPAT_KPLUGINFACTORY_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>
#include <kdemacros.h>
#include <kcomponentdata.h>
#include <kexportplugin.h>

class KAboutData;
class KPluginFactoryPrivate;
class QWidget;
namespace KParts { class Part; }

#define K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, baseFactory) \
class name : public baseFactory \
{ \
public: \
    explicit name(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0); \
    explicit name(const KAboutData &aboutData, QObject *parent = 0); \
    ~name(); \
private: \
    void init(); \
};

#define K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
name::name(const char *componentName, const char *catalogName, QObject *parent) \
    : baseFactory(componentName, catalogName, parent) \
{ \
    init(); \
} \
name::name(const KAboutData &aboutData, QObject *parent) \
    : baseFactory(aboutData, parent) \
{ \
    init(); \
} \
name::~name() {} \
void name::init() \
{ \
    pluginRegistrations \
}

#define K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
    K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, baseFactory) \
    K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations)

#define K_PLUGIN_FACTORY(name, pluginRegistrations) K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)
#define K_PLUGIN_FACTORY_DECLARATION(name) K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, KPluginFactory)
#define K_PLUGIN_FACTORY_DEFINITION(name, pluginRegistrations) K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)

class KDECORE_EXPORT KPluginFactory : public QObject
{
    Q_OBJECT
public:
    explicit KPluginFactory(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0);
    explicit KPluginFactory(const KAboutData &aboutData, QObject *parent = 0);
    explicit KPluginFactory(const KAboutData *aboutData, QObject *parent = 0);
    virtual ~KPluginFactory();

    KComponentData componentData() const;

    template <typename T>
    T *create(QObject *parent = 0, const QVariantList &args = QVariantList());
    template <typename T>
    T *create(const QString &keyword, QObject *parent = 0, const QVariantList &args = QVariantList());
    template <typename T>
    T *create(QWidget *parentWidget, QObject *parent, const QString &keyword = QString(), const QVariantList &args = QVariantList());
    template <typename T>
    T *create(QObject *parent, const QStringList &args)
    {
        return create<T>(parent, stringListToVariantList(args));
    }

    virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword);

    typedef QObject *(*CreateInstanceFunction)(QWidget *, QObject *, const QVariantList &);

Q_SIGNALS:
    void objectCreated(QObject *object);

protected:
    template <class T>
    void registerPlugin(const QString &keyword = QString(), CreateInstanceFunction instanceFunction
                        = InheritanceChecker<T>().createInstanceFunction(reinterpret_cast<T *>(0)))
    {
        registerPlugin(keyword, &T::staticMetaObject, instanceFunction);
    }

    QVariantList stringListToVariantList(const QStringList &list);
    QStringList variantListToStringList(const QVariantList &list);
    void setComponentData(const KComponentData &componentData);

    template <class impl, class ParentType>
    static QObject *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        Q_UNUSED(parentWidget)
        ParentType *p = 0;
        if (parent) {
            p = qobject_cast<ParentType *>(parent);
            Q_ASSERT(p);
        }
        return new impl(p, args);
    }

    template <class impl>
    static QObject *createPartInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        return new impl(parentWidget, parent, args);
    }

    template <class impl>
    struct InheritanceChecker
    {
        CreateInstanceFunction createInstanceFunction(KParts::Part *) { return &createPartInstance<impl>; }
        CreateInstanceFunction createInstanceFunction(QWidget *) { return &createInstance<impl, QWidget>; }
        CreateInstanceFunction createInstanceFunction(...) { return &createInstance<impl, QObject>; }
    };

private:
    void registerPlugin(const QString &keyword, const QMetaObject *metaObject, CreateInstanceFunction instanceFunction);
    KPluginFactoryPrivate *const d;
};

template <typename T>
inline T *KPluginFactory::create(QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent) : 0, parent, args, QString());
    T *t = qobject_cast<T *>(o);
    if (!t)
        delete o;
    return t;
}

template <typename T>
inline T *KPluginFactory::create(const QString &keyword, QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent) : 0, parent, args, keyword);
    T *t = qobject_cast<T *>(o);
    if (!t)
        delete o;
    return t;
}

template <typename T>
inline T *KPluginFactory::create(QWidget *parentWidget, QObject *parent, const QString &keyword, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parentWidget, parent, args, keyword);
    T *t = qobject_cast<T *>(o);
    if (!t)
        delete o;
    return t;
}

typedef KPluginFactory KLibFactory;

#endif
