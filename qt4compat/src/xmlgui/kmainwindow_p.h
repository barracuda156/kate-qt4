/* qt4compat: kmainwindow_p.h - private data shared with KXmlGuiWindow */
#ifndef KDECOMPAT_KMAINWINDOW_P_H
#define KDECOMPAT_KMAINWINDOW_P_H

#include <kconfiggroup.h>
#include <QtCore/QPointer>
#include <QtCore/QRect>

#define K_D(Class) Class##Private * const d = k_func()

class QTimer;
class KHelpMenu;
class KMainWindow;

class KMainWindowPrivate
{
public:
    KMainWindowPrivate() {}
    virtual ~KMainWindowPrivate() {}

    bool autoSaveSettings;
    bool settingsDirty;
    bool autoSaveWindowSize;
    bool care_about_geometry;
    bool sizeApplied;
    bool shuttingDown;
    KConfigGroup autoSaveGroup;
    QTimer *settingsTimer;
    QTimer *sizeTimer;
    QRect defaultWindowSize;
    KHelpMenu *helpMenu;
    KMainWindow *q;
    QString dbusName;
    bool letDirtySettings;

    void init(KMainWindow *_q);

    enum CallCompression {
        NoCompressCalls = 0,
        CompressCalls
    };
    void setSettingsDirty(CallCompression callCompression = NoCompressCalls);
    void setSizeDirty();
};

#endif
