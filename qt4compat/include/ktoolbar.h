/* qt4compat: ktoolbar.h - KToolBar on QToolBar (kdelibs 4.14 API subset, no drag-and-drop editing) */
#ifndef KDECOMPAT_KTOOLBAR_H
#define KDECOMPAT_KTOOLBAR_H

#include <QtGui/QToolBar>
#include <kdemacros.h>

class QDomElement;
class KConfigGroup;
class KConfig;
class KMainWindow;
class KXMLGUIClient;

class KDEUI_EXPORT KToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit KToolBar(QWidget *parent, bool isMainToolBar = false, bool readConfig = true);
    explicit KToolBar(const QString &objectName, QWidget *parent, bool readConfig = true);
    KToolBar(const QString &objectName, QMainWindow *parentWindow, Qt::ToolBarArea area, bool newLine = false,
             bool isMainToolBar = false, bool readConfig = true);
    virtual ~KToolBar();

    KMainWindow *mainWindow() const;
    void setIconDimensions(int size);
    int iconSizeDefault() const;
    void setContextMenuEnabled(bool enable = true);
    bool contextMenuEnabled() const;
    void saveSettings(KConfigGroup &cg);
    void applySettings(const KConfigGroup &cg, bool forceGlobal = false);
    void setXMLGUIClient(KXMLGUIClient *client);
    void addXMLGUIClient(KXMLGUIClient *client);
    void removeXMLGUIClient(KXMLGUIClient *client);
    void loadState(const QDomElement &element);
    void saveState(QDomElement &element) const;
    bool eventFilter(QObject *watched, QEvent *event);

    static Qt::ToolButtonStyle toolButtonStyleSetting();
    static bool toolBarsEditable();
    static void setToolBarsEditable(bool editable);
    static bool toolBarsLocked();
    static void setToolBarsLocked(bool locked);

protected Q_SLOTS:
    virtual void slotMovableChanged(bool movable);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *);

private Q_SLOTS:
    void slotContextTextPosition(QAction *action);
    void slotContextIconSize(QAction *action);
    void slotLockToolBars(bool lock);

private:
    void init(bool readConfig, bool isMainToolBar);
    class Private;
    Private *const d;
};

#endif
