/* qt4compat: kactioncollection.cpp */
#include <kactioncollection.h>

#include <kaction.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kxmlguiclient.h>

#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QWidget>

class KActionCollectionPrivate
{
public:
    KActionCollectionPrivate() : m_parentGUIClient(0), configIsGlobal(false), unnamedCounter(0) {}

    static QList<KActionCollection *> s_allCollections;

    KComponentData m_componentData;
    const KXMLGUIClient *m_parentGUIClient;
    QMap<QString, QAction *> actionByName;
    QList<QAction *> actions;
    QList<QWidget *> associatedWidgets;
    QString configGroup;
    bool configIsGlobal;
    int unnamedCounter;
};

QList<KActionCollection *> KActionCollectionPrivate::s_allCollections;

KActionCollection::KActionCollection(QObject *parent, const KComponentData &cData)
    : QObject(parent), d(new KActionCollectionPrivate)
{
    d->m_componentData = cData.isValid() ? cData : KGlobal::mainComponent();
    init();
}

KActionCollection::KActionCollection(const KXMLGUIClient *parent)
    : QObject(0), d(new KActionCollectionPrivate)
{
    d->m_parentGUIClient = parent;
    d->m_componentData = parent->componentData();
    init();
}

void KActionCollection::init()
{
    d->configGroup = QLatin1String("Shortcuts");
    KActionCollectionPrivate::s_allCollections.append(this);
}

KActionCollection::~KActionCollection()
{
    KActionCollectionPrivate::s_allCollections.removeAll(this);
    delete d;
}

void KActionCollection::clear()
{
    d->actionByName.clear();
    const QList<QAction *> list = d->actions;
    d->actions.clear();
    foreach (QAction *a, list) {
        disconnect(a, 0, this, 0);
        emit removed(a);
        delete a;
    }
}

QAction *KActionCollection::action(const QString &name) const
{
    QAction *action = 0;
    if (!name.isEmpty())
        action = d->actionByName.value(name);
    return action;
}

QAction *KActionCollection::action(int index) const
{
    return d->actions.value(index);
}

int KActionCollection::count() const
{
    return d->actions.count();
}

bool KActionCollection::isEmpty() const
{
    return count() == 0;
}

void KActionCollection::setComponentData(const KComponentData &cData)
{
    if (count() > 0)
        kWarning() << "KActionCollection::setComponentData called after actions were added";
    if (cData.isValid())
        d->m_componentData = cData;
    else
        d->m_componentData = KGlobal::mainComponent();
}

KComponentData KActionCollection::componentData() const
{
    return d->m_componentData;
}

const KXMLGUIClient *KActionCollection::parentGUIClient() const
{
    return d->m_parentGUIClient;
}

QList<QAction *> KActionCollection::actions() const
{
    return d->actions;
}

const QList<QAction *> KActionCollection::actionsWithoutGroup() const
{
    QList<QAction *> ret;
    foreach (QAction *action, d->actions)
        if (!action->actionGroup())
            ret.append(action);
    return ret;
}

const QList<QActionGroup *> KActionCollection::actionGroups() const
{
    QSet<QActionGroup *> set;
    foreach (QAction *action, d->actions)
        if (action->actionGroup())
            set.insert(action->actionGroup());
    return set.toList();
}

KAction *KActionCollection::addAction(const QString &name, KAction *action)
{
    QAction *ret = addAction(name, static_cast<QAction *>(action));
    Q_ASSERT(ret == action);
    Q_UNUSED(ret)
    return action;
}

QAction *KActionCollection::addAction(const QString &name, QAction *action)
{
    if (!action)
        return action;

    const QString objectName = action->objectName();
    QString indexName = name;

    if (indexName.isEmpty()) {
        indexName = objectName;
        if (indexName.isEmpty())
            indexName = QString::fromLatin1("unnamed-%1").arg(++d->unnamedCounter);
        action->setObjectName(indexName);
    } else {
        action->setObjectName(indexName);
    }

    /* an action with the same name replaces the old one */
    QAction *old = d->actionByName.value(indexName);
    if (old == action)
        return action;
    if (old)
        takeAction(old);

    /* an action may only be in one collection */
    foreach (KActionCollection *collection, KActionCollectionPrivate::s_allCollections) {
        if (collection != this && collection->d->actions.contains(action))
            collection->takeAction(action);
    }

    d->actionByName.insert(indexName, action);
    d->actions.append(action);

    foreach (QWidget *widget, d->associatedWidgets)
        widget->addAction(action);

    connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(actionDestroyed(QObject*)));
    connect(action, SIGNAL(hovered()), this, SLOT(slotActionHovered()));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotActionTriggered()));

    emit inserted(action);
    return action;
}

void KActionCollection::removeAction(QAction *action)
{
    delete takeAction(action);
}

QAction *KActionCollection::takeAction(QAction *action)
{
    if (!d->actions.contains(action))
        return 0;
    d->actions.removeAll(action);
    QMap<QString, QAction *>::Iterator it = d->actionByName.begin();
    while (it != d->actionByName.end()) {
        if (it.value() == action)
            it = d->actionByName.erase(it);
        else
            ++it;
    }
    foreach (QWidget *widget, d->associatedWidgets)
        widget->removeAction(action);
    disconnect(action, 0, this, 0);
    emit removed(action);
    return action;
}

KAction *KActionCollection::addAction(KStandardAction::StandardAction actionType, const QObject *receiver, const char *member)
{
    KAction *action = KStandardAction::create(actionType, receiver, member, this);
    return action;
}

KAction *KActionCollection::addAction(KStandardAction::StandardAction actionType, const QString &name, const QObject *receiver, const char *member)
{
    KAction *action = KStandardAction::create(actionType, receiver, member, this);
    /* create() already added it under the standard name; re-register under the requested one */
    addAction(name, action);
    return action;
}

KAction *KActionCollection::addAction(const QString &name, const QObject *receiver, const char *member)
{
    KAction *a = new KAction(this);
    if (receiver && member)
        connect(a, SIGNAL(triggered(bool)), receiver, member);
    addAction(name, a);
    return a;
}

void KActionCollection::actionDestroyed(QObject *object)
{
    QAction *action = static_cast<QAction *>(object);
    d->actions.removeAll(action);
    QMap<QString, QAction *>::Iterator it = d->actionByName.begin();
    while (it != d->actionByName.end()) {
        if (it.value() == action)
            it = d->actionByName.erase(it);
        else
            ++it;
    }
    emit removed(action);
}

void KActionCollection::associatedWidgetDestroyed(QObject *object)
{
    d->associatedWidgets.removeAll(static_cast<QWidget *>(object));
}

void KActionCollection::slotActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        emit actionTriggered(action);
}

void KActionCollection::slotActionHighlighted()
{
    slotActionHovered();
}

void KActionCollection::slotActionHovered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        emit actionHovered(action);
        emit actionHighlighted(action);
    }
}

const QList<KActionCollection *> &KActionCollection::allCollections()
{
    return KActionCollectionPrivate::s_allCollections;
}

void KActionCollection::associateWidget(QWidget *widget) const
{
    foreach (QAction *action, actions()) {
        if (!widget->actions().contains(action))
            widget->addAction(action);
    }
}

void KActionCollection::addAssociatedWidget(QWidget *widget)
{
    if (!d->associatedWidgets.contains(widget)) {
        widget->addActions(actions());
        d->associatedWidgets.append(widget);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(associatedWidgetDestroyed(QObject*)));
    }
}

void KActionCollection::removeAssociatedWidget(QWidget *widget)
{
    foreach (QAction *action, actions())
        widget->removeAction(action);
    d->associatedWidgets.removeAll(widget);
    disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(associatedWidgetDestroyed(QObject*)));
}

QList<QWidget *> KActionCollection::associatedWidgets() const
{
    return d->associatedWidgets;
}

void KActionCollection::clearAssociatedWidgets()
{
    foreach (QWidget *widget, d->associatedWidgets)
        foreach (QAction *action, actions())
            widget->removeAction(action);
    d->associatedWidgets.clear();
}

QString KActionCollection::configGroup() const
{
    return d->configGroup;
}

void KActionCollection::setConfigGroup(const QString &group)
{
    d->configGroup = group;
}

bool KActionCollection::configIsGlobal() const
{
    return d->configIsGlobal;
}

void KActionCollection::setConfigGlobal(bool global)
{
    d->configIsGlobal = global;
}

void KActionCollection::importGlobalShortcuts(KConfigGroup *)
{
}

void KActionCollection::exportGlobalShortcuts(KConfigGroup *, bool) const
{
}

void KActionCollection::readSettings(KConfigGroup *config)
{
    KConfigGroup cg(KGlobal::config(), configGroup());
    if (config)
        cg = *config;
    if (!cg.exists())
        return;

    foreach (QAction *action, actions()) {
        KAction *kaction = qobject_cast<KAction *>(action);
        if (!kaction || !kaction->isShortcutConfigurable())
            continue;
        const QString actionName = action->objectName();
        const QString entry = cg.readEntry(actionName, QString());
        if (!entry.isEmpty()) {
            if (entry == QLatin1String("none"))
                kaction->setShortcut(KShortcut(), KAction::ActiveShortcut);
            else
                kaction->setShortcut(KShortcut(entry), KAction::ActiveShortcut);
        } else {
            kaction->setShortcut(kaction->shortcut(KAction::DefaultShortcut), KAction::ActiveShortcut);
        }
    }
}

void KActionCollection::writeSettings(KConfigGroup *config, bool writeDefaults, QAction *oneAction) const
{
    KConfigGroup cg(KGlobal::config(), configGroup());
    if (config)
        cg = *config;

    QList<QAction *> writeActions;
    if (oneAction)
        writeActions.append(oneAction);
    else
        writeActions = actions();

    KConfigGroup::WriteConfigFlags flags = KConfigGroup::Persistent;
    if (configIsGlobal())
        flags |= KConfigGroup::Global;

    foreach (QAction *action, writeActions) {
        KAction *kaction = qobject_cast<KAction *>(action);
        if (!kaction || !kaction->isShortcutConfigurable())
            continue;
        const QString actionName = action->objectName();
        const bool sameAsDefault = (kaction->shortcut(KAction::ActiveShortcut) == kaction->shortcut(KAction::DefaultShortcut));
        if (writeDefaults || !sameAsDefault) {
            QString s = kaction->shortcut(KAction::ActiveShortcut).toString();
            if (s.isEmpty())
                s = QLatin1String("none");
            cg.writeEntry(actionName, s, flags);
        } else if (cg.hasKey(actionName)) {
            cg.deleteEntry(actionName, flags);
        }
    }
    cg.sync();
}
