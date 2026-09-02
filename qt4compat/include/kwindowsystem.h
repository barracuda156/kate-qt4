/* qt4compat: kwindowsystem.h - window management through QWidget, no X11 */
#ifndef KDECOMPAT_KWINDOWSYSTEM_H
#define KDECOMPAT_KWINDOWSYSTEM_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <kdemacros.h>

class KDEUI_EXPORT KWindowInfo
{
public:
    KWindowInfo() : m_win(0) {}
    explicit KWindowInfo(WId win, unsigned long = 0, unsigned long = 0) : m_win(win) {}
    bool valid(bool = false) const { return QWidget::find(m_win) != 0; }
    WId win() const { return m_win; }
    QString visibleName() const;
    QString name() const;
    bool isMinimized() const;
    bool isOnDesktop(int) const { return true; }
    bool onAllDesktops() const { return true; }
    int desktop() const { return 1; }
    QRect geometry() const;
    QRect frameGeometry() const;
    WId transientFor() const { return 0; }
    WId groupLeader() const { return 0; }
private:
    WId m_win;
};

class KDEUI_EXPORT KWindowSystem : public QObject
{
    Q_OBJECT
public:
    static KWindowSystem *self();

    static QList<WId> windows();
    static KWindowInfo windowInfo(WId win, unsigned long properties, unsigned long properties2 = 0);
    static bool hasWId(WId id);
    static QList<WId> stackingOrder();
    static WId activeWindow();
    static void activateWindow(WId win, long time = 0);
    static void forceActiveWindow(WId win, long time = 0);
    static void demandAttention(WId win, bool set = true);
    static void raiseWindow(WId win);
    static void lowerWindow(WId win);
    static bool compositingActive();
    static int currentDesktop();
    static int numberOfDesktops();
    static void setCurrentDesktop(int desktop);
    static void setOnAllDesktops(WId win, bool b);
    static void setOnDesktop(WId win, int desktop);
    static void setMainWindow(QWidget *subwindow, WId mainwindow);
    static WId transientFor(WId window);
    static WId groupLeader(WId window);
    static QPixmap icon(WId win, int width = -1, int height = -1, bool scale = false);
    static void setIcons(WId win, const QPixmap &icon, const QPixmap &miniIcon);
    static void setType(WId win, int windowType);
    static void setState(WId win, unsigned long state);
    static void clearState(WId win, unsigned long state);
    static void minimizeWindow(WId win, bool animation = true);
    static void unminimizeWindow(WId win, bool animation = true);
    static QRect workArea(int desktop = -1);
    static QString desktopName(int desktop);
    static void setDesktopName(int desktop, const QString &name);
    static bool showingDesktop();
    static void allowExternalProcessWindowActivation(int pid = -1);
    static void allowMoveResize(WId win);
    static bool icccmCompliantMappingState();

Q_SIGNALS:
    void windowAdded(WId id);
    void windowRemoved(WId id);
    void activeWindowChanged(WId id);
    void desktopNamesChanged();
    void numberOfDesktopsChanged(int num);
    void currentDesktopChanged(int desktop);
    void windowChanged(WId id, unsigned int properties);
    void windowChanged(WId id);
    void strutChanged();
    void stackingOrderChanged();
    void showingDesktopChanged(bool showing);
    void compositingChanged(bool enabled);

private:
    KWindowSystem() : QObject(0) {}
};

#endif
