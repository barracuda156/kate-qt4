/* qt4compat: kmainwindow.cpp - modelled on kdelibs 4.14 kdeui/widgets/kmainwindow.cpp
 * (LGPL 2, (C) 2000 Reginald Stadlbauer, 1997 Stephan Kulow, 1997-2000 Sven Radej,
 * 1997-2000 Matthias Ettrich, 1999 Chris Schlaeger, 2002 Joseph Wenninger,
 * 2005-2006 Hamish Rodda); session management, D-Bus and X11 code removed. */
#include <kmainwindow.h>
#include "kmainwindow_p.h"

#include <kcomponentdata.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <ksharedconfig.h>
#include <kstatusbar.h>
#include <ktoolbar.h>

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDockWidget>

static QList<KMainWindow *> *memberListStorage()
{
    static QList<KMainWindow *> list;
    return &list;
}

static KMenuBar *internalMenuBar(KMainWindow *mw)
{
    foreach (QObject *child, mw->children()) {
        if (KMenuBar *mb = qobject_cast<KMenuBar *>(child))
            return mb;
    }
    return 0;
}

static KStatusBar *internalStatusBar(KMainWindow *mw)
{
    foreach (QObject *child, mw->children()) {
        if (KStatusBar *sb = qobject_cast<KStatusBar *>(child))
            return sb;
    }
    return 0;
}

KMainWindow::KMainWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), k_ptr(new KMainWindowPrivate)
{
    k_ptr->init(this);
}

KMainWindow::KMainWindow(KMainWindowPrivate &dd, QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), k_ptr(&dd)
{
    k_ptr->init(this);
}

void KMainWindowPrivate::init(KMainWindow *_q)
{
    q = _q;
    q->setAnimated(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects);
    q->setAttribute(Qt::WA_DeleteOnClose);

    helpMenu = 0;
    settingsDirty = false;
    autoSaveSettings = false;
    autoSaveWindowSize = true;
    care_about_geometry = false;
    sizeApplied = false;
    shuttingDown = false;
    letDirtySettings = true;
    settingsTimer = 0;
    sizeTimer = 0;

    memberListStorage()->append(q);

    /* give the window a unique object name, like kdelibs */
    if (q->objectName().isEmpty()) {
        int n = 1;
        for (;;) {
            const QString candidate = QString::fromLatin1("MainWindow#%1").arg(n);
            bool used = false;
            foreach (KMainWindow *w, *memberListStorage()) {
                if (w != q && w->objectName() == candidate) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                q->setObjectName(candidate);
                break;
            }
            ++n;
        }
    }

    q->setWindowTitle(KGlobal::caption());
    QObject::connect(qApp, SIGNAL(aboutToQuit()), q, SLOT(slotShuttingDown()));
    QObject::connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), q, SLOT(slotSettingsChanged(int)));
}

KMainWindow::~KMainWindow()
{
    memberListStorage()->removeAll(this);
    delete k_ptr;
}

KMenu *KMainWindow::helpMenu(const QString &aboutAppText, bool showWhatsThis)
{
    K_D(KMainWindow);
    if (!d->helpMenu) {
        if (aboutAppText.isEmpty())
            d->helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(), showWhatsThis);
        else
            d->helpMenu = new KHelpMenu(this, aboutAppText, showWhatsThis);
        if (!d->helpMenu)
            return 0;
    }
    return d->helpMenu->menu();
}

KMenu *KMainWindow::customHelpMenu(bool showWhatsThis)
{
    K_D(KMainWindow);
    if (!d->helpMenu) {
        d->helpMenu = new KHelpMenu(this, QString(), showWhatsThis);
        connect(d->helpMenu, SIGNAL(showAboutApplication()), this, SLOT(showAboutApplication()));
    }
    return d->helpMenu->menu();
}

bool KMainWindow::canBeRestored(int)
{
    return false;
}

const QString KMainWindow::classNameOfToplevel(int)
{
    return QString();
}

bool KMainWindow::restore(int, bool)
{
    return false;
}

bool KMainWindow::hasMenuBar()
{
    return internalMenuBar(this) != 0;
}

KMenuBar *KMainWindow::menuBar()
{
    KMenuBar *mb = internalMenuBar(this);
    if (!mb) {
        mb = new KMenuBar(this);
        setMenuBar(mb);
    }
    return mb;
}

KStatusBar *KMainWindow::statusBar()
{
    KStatusBar *sb = internalStatusBar(this);
    if (!sb) {
        sb = new KStatusBar(this);
        setStatusBar(sb);
    }
    return sb;
}

QList<KMainWindow *> KMainWindow::memberList()
{
    return *memberListStorage();
}

KToolBar *KMainWindow::toolBar(const QString &name)
{
    QString childName = name;
    if (childName.isEmpty())
        childName = QLatin1String("mainToolBar");
    KToolBar *tb = findChild<KToolBar *>(childName);
    if (tb)
        return tb;
    return new KToolBar(childName, this, childName == QLatin1String("mainToolBar"));
}

QList<KToolBar *> KMainWindow::toolBars() const
{
    QList<KToolBar *> ret;
    foreach (QObject *child, children())
        if (KToolBar *toolBar = qobject_cast<KToolBar *>(child))
            ret.append(toolBar);
    return ret;
}

void KMainWindow::setAutoSaveSettings(const QString &groupName, bool saveWindowSize)
{
    setAutoSaveSettings(KConfigGroup(KGlobal::config(), groupName), saveWindowSize);
}

void KMainWindow::setAutoSaveSettings(const KConfigGroup &group, bool saveWindowSize)
{
    K_D(KMainWindow);
    d->autoSaveSettings = true;
    d->autoSaveGroup = group;
    d->autoSaveWindowSize = saveWindowSize;
    if (!saveWindowSize && d->sizeTimer)
        d->sizeTimer->stop();
    applyMainWindowSettings(d->autoSaveGroup);
}

void KMainWindow::resetAutoSaveSettings()
{
    K_D(KMainWindow);
    d->autoSaveSettings = false;
    if (d->settingsTimer)
        d->settingsTimer->stop();
}

bool KMainWindow::autoSaveSettings() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings;
}

QString KMainWindow::autoSaveGroup() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup.name() : QString();
}

KConfigGroup KMainWindow::autoSaveConfigGroup() const
{
    K_D(const KMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup : KConfigGroup();
}

void KMainWindow::applyMainWindowSettings(const KConfigGroup &cg, bool force)
{
    K_D(KMainWindow);
    QWidget *focusedWidget = QApplication::focusWidget();

    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    if (!d->sizeApplied) {
        restoreWindowSize(cg);
        d->sizeApplied = true;
    }

    QStatusBar *sb = internalStatusBar(this);
    if (sb) {
        const QString entry = cg.readEntry("StatusBar", "Enabled");
        if (entry == QLatin1String("Disabled"))
            sb->hide();
        else
            sb->show();
    }

    QMenuBar *mb = internalMenuBar(this);
    if (mb) {
        const QString entry = cg.readEntry("MenuBar", "Enabled");
        if (entry == QLatin1String("Disabled"))
            mb->hide();
        else
            mb->show();
    }

    if (!autoSaveSettings() || cg.name() == autoSaveGroup()) {
        const QString entry = cg.readEntry("ToolBarsMovable", "Disabled");
        KToolBar::setToolBarsLocked(entry == QLatin1String("Disabled"));
    }

    int n = 1;
    foreach (KToolBar *toolbar, toolBars()) {
        QByteArray groupName("Toolbar");
        groupName += (toolbar->objectName().isEmpty() ? QByteArray::number(n) : QByteArray(" ").append(toolbar->objectName().toUtf8()));
        KConfigGroup toolbarGroup(&cg, groupName.constData());
        toolbar->applySettings(toolbarGroup, force);
        n++;
    }

    if (cg.hasKey("State")) {
        QByteArray state = cg.readEntry("State", QByteArray());
        state = QByteArray::fromBase64(state);
        restoreState(state);
    }

    if (focusedWidget)
        focusedWidget->setFocus();

    d->settingsDirty = false;
    d->letDirtySettings = oldLetDirtySettings;
}

void KMainWindow::saveMainWindowSettings(const KConfigGroup &_cg)
{
    K_D(KMainWindow);
    KConfigGroup cg(_cg);

    if (d->autoSaveWindowSize)
        saveWindowSize(cg);

    const QByteArray sState = saveState();
    cg.writeEntry("State", sState.toBase64());

    QStatusBar *sb = internalStatusBar(this);
    if (sb) {
        if (!cg.hasDefault("StatusBar") && !sb->isHidden())
            cg.revertToDefault("StatusBar");
        else
            cg.writeEntry("StatusBar", sb->isHidden() ? "Disabled" : "Enabled");
    }

    QMenuBar *mb = internalMenuBar(this);
    if (mb) {
        if (!cg.hasDefault("MenuBar") && !mb->isHidden())
            cg.revertToDefault("MenuBar");
        else
            cg.writeEntry("MenuBar", mb->isHidden() ? "Disabled" : "Enabled");
    }

    if (!autoSaveSettings() || cg.name() == autoSaveGroup()) {
        if (!cg.hasDefault("ToolBarsMovable") && !KToolBar::toolBarsLocked())
            cg.revertToDefault("ToolBarsMovable");
        else
            cg.writeEntry("ToolBarsMovable", KToolBar::toolBarsLocked() ? "Disabled" : "Enabled");
    }

    int n = 1;
    foreach (KToolBar *toolbar, toolBars()) {
        QByteArray groupName("Toolbar");
        groupName += (toolbar->objectName().isEmpty() ? QByteArray::number(n) : QByteArray(" ").append(toolbar->objectName().toUtf8()));
        KConfigGroup toolbarGroup(&cg, groupName.constData());
        toolbar->saveSettings(toolbarGroup);
        n++;
    }
}

bool KMainWindow::initialGeometrySet() const
{
    K_D(const KMainWindow);
    return d->care_about_geometry;
}

void KMainWindow::ignoreInitialGeometry()
{
    K_D(KMainWindow);
    d->care_about_geometry = false;
}

QString KMainWindow::dbusName() const
{
    K_D(const KMainWindow);
    return d->dbusName;
}

static QString makeStandardCaption(const QString &userCaption, bool modified)
{
    QString caption = userCaption;
#ifndef Q_OS_MAC
    const QString appName = KGlobal::caption();
    if (caption.isEmpty())
        caption = appName;
    else if (!appName.isEmpty())
        caption = caption + QString::fromUtf8(" \xe2\x80\x93 ") + appName;
#endif
    Q_UNUSED(modified)
    return caption + QLatin1String("[*]");
}

void KMainWindow::setCaption(const QString &caption)
{
    setPlainCaption(makeStandardCaption(caption, false));
    setWindowModified(false);
}

void KMainWindow::setCaption(const QString &caption, bool modified)
{
    setPlainCaption(makeStandardCaption(caption, modified));
    setWindowModified(modified);
}

void KMainWindow::setPlainCaption(const QString &caption)
{
    setWindowTitle(caption);
}

void KMainWindow::appHelpActivated(void)
{
    K_D(KMainWindow);
    if (!d->helpMenu)
        d->helpMenu = new KHelpMenu(this);
    d->helpMenu->appHelpActivated();
}

void KMainWindow::setSettingsDirty()
{
    K_D(KMainWindow);
    d->setSettingsDirty();
}

void KMainWindowPrivate::setSettingsDirty(CallCompression callCompression)
{
    if (!letDirtySettings)
        return;
    settingsDirty = true;
    if (autoSaveSettings) {
        if (callCompression == CompressCalls) {
            if (!settingsTimer) {
                settingsTimer = new QTimer(q);
                settingsTimer->setInterval(500);
                settingsTimer->setSingleShot(true);
                QObject::connect(settingsTimer, SIGNAL(timeout()), q, SLOT(saveAutoSaveSettings()));
            }
            settingsTimer->start();
        } else {
            q->saveAutoSaveSettings();
        }
    }
}

void KMainWindowPrivate::setSizeDirty()
{
    if (autoSaveWindowSize) {
        if (!sizeTimer) {
            sizeTimer = new QTimer(q);
            sizeTimer->setInterval(500);
            sizeTimer->setSingleShot(true);
            QObject::connect(sizeTimer, SIGNAL(timeout()), q, SLOT(slotSaveAutoSaveSize()));
        }
        sizeTimer->start();
    }
}

bool KMainWindow::event(QEvent *ev)
{
    K_D(KMainWindow);
    if (d->autoSaveSettings) {
        switch (ev->type()) {
        case QEvent::Resize:
            if (!isMaximized() && !isFullScreen())
                d->defaultWindowSize = QRect(QPoint(0, 0), size());
            d->setSizeDirty();
            break;
        case QEvent::ChildPolished: {
            QChildEvent *event = static_cast<QChildEvent *>(ev);
            QDockWidget *dock = qobject_cast<QDockWidget *>(event->child());
            if (dock) {
                connect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(setSettingsDirty()));
                connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(setSettingsDirty()));
                connect(dock, SIGNAL(visibilityChanged(bool)), this, SLOT(setSettingsDirty()));
            }
            break;
        }
        default:
            break;
        }
    }
    return QMainWindow::event(ev);
}

void KMainWindow::closeEvent(QCloseEvent *e)
{
    K_D(KMainWindow);

    if (d->settingsDirty && d->autoSaveSettings)
        saveAutoSaveSettings();

    if (queryClose()) {
        e->accept();

        int not_withdrawn = 0;
        foreach (KMainWindow *mw, memberList()) {
            if (!mw->isHidden() && mw->isTopLevel() && mw != this)
                not_withdrawn++;
        }
        if (!not_withdrawn) {
            // last window closed: quit the application, as kdelibs does.
            // Applications keep quitOnLastWindowClosed off (dialogs shown
            // before the first window would end them otherwise), so nothing
            // else quits; on macOS a window-less application also loses
            // every menu but the application menu.
            if (queryExit()) {
                d->shuttingDown = true;
                QCoreApplication::instance()->quit();
            } else {
                e->ignore();
            }
        }
    } else {
        e->ignore();
    }
}

bool KMainWindow::queryExit()
{
    return true;
}

bool KMainWindow::queryClose()
{
    return true;
}

void KMainWindow::saveGlobalProperties(KConfig *)
{
}

void KMainWindow::readGlobalProperties(KConfig *)
{
}

void KMainWindow::savePropertiesInternal(KConfig *config, int number)
{
    KConfigGroup cg(config, QString::fromLatin1("WindowProperties%1").arg(number));
    cg.writeEntry("ObjectName", objectName());
    cg.writeEntry("ClassName", QString::fromLatin1(metaObject()->className()));
    saveMainWindowSettings(cg);
    KConfigGroup grp(config, QString::number(number));
    saveProperties(grp);
}

bool KMainWindow::readPropertiesInternal(KConfig *config, int number)
{
    K_D(KMainWindow);
    if (!config)
        return false;
    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    KConfigGroup cg(config, QString::fromLatin1("WindowProperties%1").arg(number));
    if (cg.hasKey("ObjectName"))
        setObjectName(cg.readEntry("ObjectName", QString()));
    d->sizeApplied = false;
    applyMainWindowSettings(cg);

    KConfigGroup grp(config, QString::number(number));
    readProperties(grp);

    d->letDirtySettings = oldLetDirtySettings;
    return true;
}

bool KMainWindow::settingsDirty() const
{
    K_D(const KMainWindow);
    return d->settingsDirty;
}

void KMainWindow::saveWindowSize(const KConfigGroup &_cg) const
{
    K_D(const KMainWindow);
    KConfigGroup cg(_cg);
    const int scnum = QApplication::desktop()->screenNumber(window());
    const QRect desk = QApplication::desktop()->screenGeometry(scnum);

    QSize sizeToSave = size();
    if ((isMaximized() || isFullScreen()) && d->defaultWindowSize.isValid())
        sizeToSave = d->defaultWindowSize.size();

    cg.writeEntry(QString::fromLatin1("Window-Maximized %1x%2").arg(desk.width()).arg(desk.height()), isMaximized());
    cg.writeEntry(QString::fromLatin1("Width %1").arg(desk.width()), sizeToSave.width());
    cg.writeEntry(QString::fromLatin1("Height %1").arg(desk.height()), sizeToSave.height());
}

void KMainWindow::restoreWindowSize(const KConfigGroup &config)
{
    K_D(KMainWindow);
    if (d->care_about_geometry) {
        parseGeometry(true);
        return;
    }
    const int scnum = QApplication::desktop()->screenNumber(window());
    const QRect desk = QApplication::desktop()->screenGeometry(scnum);

    const QSize size(config.readEntry(QString::fromLatin1("Width %1").arg(desk.width()), 0),
                     config.readEntry(QString::fromLatin1("Height %1").arg(desk.height()), 0));
    if (!size.isEmpty()) {
        d->defaultWindowSize = QRect(QPoint(0, 0), size);
        resize(size);
    }
    if (config.readEntry(QString::fromLatin1("Window-Maximized %1x%2").arg(desk.width()).arg(desk.height()), false))
        setWindowState(windowState() | Qt::WindowMaximized);
}

void KMainWindow::parseGeometry(bool)
{
}

void KMainWindow::showAboutApplication()
{
}

void KMainWindow::saveAutoSaveSettings()
{
    K_D(KMainWindow);
    if (!d->autoSaveSettings)
        return;
    saveMainWindowSettings(d->autoSaveGroup);
    d->autoSaveGroup.sync();
    d->settingsDirty = false;
}

void KMainWindow::slotShuttingDown()
{
    K_D(KMainWindow);
    d->shuttingDown = true;
    if (d->settingsDirty && d->autoSaveSettings)
        saveAutoSaveSettings();
}

void KMainWindow::slotSettingsChanged(int)
{
    setAnimated(KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects);
}

void KMainWindow::slotSaveAutoSaveSize()
{
    K_D(KMainWindow);
    if (d->autoSaveGroup.isValid())
        saveWindowSize(d->autoSaveGroup);
}
