/* qt4compat: kactioncollection.h (kdelibs 4.14 API, no global shortcuts) */
#ifndef KDECOMPAT_KACTIONCOLLECTION_H
#define KDECOMPAT_KACTIONCOLLECTION_H

#include <QtCore/QObject>
#include <kcomponentdata.h>
#include <kdemacros.h>
#include <kstandardaction.h>

class QAction;
class QActionGroup;
class QString;
class QWidget;
class KAction;
class KConfigGroup;
class KXMLGUIClient;
class KActionCollectionPrivate;

class KDEUI_EXPORT KActionCollection : public QObject
{
    friend class KXMLGUIClient;
    Q_OBJECT
    Q_PROPERTY(QString configGroup READ configGroup WRITE setConfigGroup)
    Q_PROPERTY(bool configIsGlobal READ configIsGlobal WRITE setConfigGlobal)
public:
    explicit KActionCollection(QObject *parent, const KComponentData &cData = KComponentData());
    virtual ~KActionCollection();

    static const QList<KActionCollection *> &allCollections();

    void clear();
    void associateWidget(QWidget *widget) const;
    void addAssociatedWidget(QWidget *widget);
    void removeAssociatedWidget(QWidget *widget);
    QList<QWidget *> associatedWidgets() const;
    void clearAssociatedWidgets();

    QString configGroup() const;
    bool configIsGlobal() const;
    void setConfigGroup(const QString &group);
    void setConfigGlobal(bool global);
    void readSettings(KConfigGroup *config = 0);
    void importGlobalShortcuts(KConfigGroup *config);
    void exportGlobalShortcuts(KConfigGroup *config, bool writeDefaults = false) const;
    void writeSettings(KConfigGroup *config = 0, bool writeDefaults = false, QAction *oneAction = 0) const;

    int count() const;
    bool isEmpty() const;
    QAction *action(int index) const;
    QAction *action(const QString &name) const;
    QList<QAction *> actions() const;
    const QList<QAction *> actionsWithoutGroup() const;
    const QList<QActionGroup *> actionGroups() const;

    void setComponentData(const KComponentData &componentData);
    KComponentData componentData() const;
    const KXMLGUIClient *parentGUIClient() const;

Q_SIGNALS:
    void inserted(QAction *action);
    void removed(QAction *action);
    void actionHighlighted(QAction *action);
    void actionHovered(QAction *action);
    void actionTriggered(QAction *action);

protected Q_SLOTS:
    virtual void slotActionTriggered();
    virtual void slotActionHighlighted();

private Q_SLOTS:
    void slotActionHovered();
    void actionDestroyed(QObject *object);
    void associatedWidgetDestroyed(QObject *object);

public:
    QAction *addAction(const QString &name, QAction *action);
    KAction *addAction(const QString &name, KAction *action);
    void removeAction(QAction *action);
    QAction *takeAction(QAction *action);
    KAction *addAction(KStandardAction::StandardAction actionType, const QObject *receiver = 0, const char *member = 0);
    KAction *addAction(KStandardAction::StandardAction actionType, const QString &name, const QObject *receiver = 0, const char *member = 0);
    KAction *addAction(const QString &name, const QObject *receiver = 0, const char *member = 0);

    template<class ActionType>
    ActionType *add(const QString &name, const QObject *receiver = 0, const char *member = 0)
    {
        ActionType *a = new ActionType(this);
        if (receiver && member)
            connect(a, SIGNAL(triggered(bool)), receiver, member);
        addAction(name, a);
        return a;
    }

private:
    KActionCollection(const KXMLGUIClient *parent);
    void init();
    KActionCollectionPrivate *const d;
};

#endif
