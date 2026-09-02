/* qt4compat: kmainwindow.h - KMainWindow on QMainWindow (kdelibs 4.14 API, no session
 * management, no D-Bus). Written new for qt4compat; API follows kdelibs. */
#ifndef KDECOMPAT_KMAINWINDOW_H
#define KDECOMPAT_KMAINWINDOW_H

#include <QtCore/QList>
#include <QtCore/QMetaClassInfo>
#include <QtGui/QMainWindow>
#include <kdemacros.h>

class KMenu;
class KConfig;
class KConfigGroup;
class KStatusBar;
class KMenuBar;
class KMainWindowPrivate;
class KToolBar;

#define KDEUI_DECLARE_PRIVATE(classname) \
    inline classname ## Private *k_func() { return reinterpret_cast<classname ## Private *>(k_ptr); } \
    inline const classname ## Private *k_func() const { return reinterpret_cast<classname ## Private *>(k_ptr); } \
    friend class classname ## Private;

#define KDE_DEFAULT_WINDOWFLAGS 0

class KDEUI_EXPORT KMainWindow : public QMainWindow
{
    KDEUI_DECLARE_PRIVATE(KMainWindow)
    Q_OBJECT
    Q_PROPERTY(bool hasMenuBar READ hasMenuBar)
    Q_PROPERTY(bool autoSaveSettings READ autoSaveSettings)
    Q_PROPERTY(QString autoSaveGroup READ autoSaveGroup)
    Q_PROPERTY(bool initialGeometrySet READ initialGeometrySet)
public:
    explicit KMainWindow(QWidget *parent = 0, Qt::WindowFlags f = KDE_DEFAULT_WINDOWFLAGS);
    virtual ~KMainWindow();

    KMenu *helpMenu(const QString &aboutAppText = QString(), bool showWhatsThis = true);
    KMenu *customHelpMenu(bool showWhatsThis = true);

    static bool canBeRestored(int number);
    static const QString classNameOfToplevel(int number);
    bool restore(int number, bool show = true);

    bool hasMenuBar();
    KMenuBar *menuBar();
    KStatusBar *statusBar();
    static QList<KMainWindow *> memberList();
    KToolBar *toolBar(const QString &name = QString());
    QList<KToolBar *> toolBars() const;

    void setAutoSaveSettings(const QString &groupName = QLatin1String("MainWindow"), bool saveWindowSize = true);
    void setAutoSaveSettings(const KConfigGroup &group, bool saveWindowSize = true);
    void resetAutoSaveSettings();
    bool autoSaveSettings() const;
    QString autoSaveGroup() const;
    KConfigGroup autoSaveConfigGroup() const;
    virtual void applyMainWindowSettings(const KConfigGroup &config, bool forceGlobal = false);
    void saveMainWindowSettings(const KConfigGroup &config);
    bool initialGeometrySet() const;
    void ignoreInitialGeometry();
    QString dbusName() const;

public Q_SLOTS:
    virtual void setCaption(const QString &caption);
    virtual void setCaption(const QString &caption, bool modified);
    virtual void setPlainCaption(const QString &caption);
    void appHelpActivated(void);
    void setSettingsDirty();

protected:
    virtual bool event(QEvent *event);
    virtual void closeEvent(QCloseEvent *);
    virtual bool queryExit();
    virtual bool queryClose();
    virtual void saveProperties(KConfigGroup &) {}
    virtual void readProperties(const KConfigGroup &) {}
    virtual void saveGlobalProperties(KConfig *sessionConfig);
    virtual void readGlobalProperties(KConfig *sessionConfig);
    void savePropertiesInternal(KConfig *, int);
    bool readPropertiesInternal(KConfig *, int);
    bool settingsDirty() const;
    void saveWindowSize(const KConfigGroup &config) const;
    void restoreWindowSize(const KConfigGroup &config);
    void parseGeometry(bool parsewidth);

protected Q_SLOTS:
    virtual void showAboutApplication();
    void saveAutoSaveSettings();

private Q_SLOTS:
    void slotShuttingDown();
    void slotSettingsChanged(int category);
    void slotSaveAutoSaveSize();

protected:
    KMainWindow(KMainWindowPrivate &dd, QWidget *parent, Qt::WindowFlags f);
    KMainWindowPrivate *const k_ptr;
};

#define RESTORE(type) { int n = 1;\
    while (KMainWindow::canBeRestored(n)){\
      (new type)->restore(n);\
      n++;}}

template <typename T>
inline void kRestoreMainWindows()
{
    for (int n = 1; KMainWindow::canBeRestored(n); ++n) {
        const QString className = KMainWindow::classNameOfToplevel(n);
        if (className == QLatin1String(T::staticMetaObject.className()))
            (new T)->restore(n);
    }
}

#endif
