/* qt4compat: kglobalsettings.h - desktop-wide settings, with sane fixed defaults */
#ifndef KDECOMPAT_KGLOBALSETTINGS_H
#define KDECOMPAT_KGLOBALSETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <kdemacros.h>
#include <ksharedptr.h>

class KSharedConfig;
class KUrl;
class QRect;
class QPoint;
class QWidget;
typedef KSharedPtr<KSharedConfig> KSharedConfigPtr;

class KDEUI_EXPORT KGlobalSettings : public QObject
{
    Q_OBJECT
public:
    static KGlobalSettings *self();

    enum ChangeActivationFlag { ApplySettings = 0x01, ListenForChanges = 0x02 };
    Q_DECLARE_FLAGS(ActivateOptions, ChangeActivationFlag)
    void activate();
    void activate(ActivateOptions options);

    static int dndEventDelay();
    static bool singleClick();
    static bool changeCursorOverIcon();
    static int autoSelectDelay();
    static bool showContextMenusOnPress();

    enum Completion {
        CompletionNone = 1,
        CompletionAuto,
        CompletionMan,
        CompletionShell,
        CompletionPopup,
        CompletionPopupAuto
    };
    static Completion completionMode();

    static bool insertTearOffHandle();
    static QColor toolBarHighlightColor();
    static QColor inactiveTitleColor();
    static QColor inactiveTextColor();
    static QColor activeTitleColor();
    static QColor activeTextColor();
    static int contrast();
    static qreal contrastF();
    static qreal contrastF(const KSharedConfigPtr &config);
    static bool shadeSortColumn();
    static bool allowDefaultBackgroundImages();

    static QFont generalFont();
    static QFont fixedFont();
    static QFont toolBarFont();
    static QFont menuFont();
    static QFont windowTitleFont();
    static QFont taskbarFont();
    static QFont largeFont(const QString &text = QString());
    static QFont smallestReadableFont();

    static bool isMultiHead();
    static bool wheelMouseZooms();
    static QRect splashScreenDesktopGeometry();
    static QRect desktopGeometry(const QPoint &point);
    static QRect desktopGeometry(const QWidget *w);
    static bool showIconsOnPushButtons();
    static bool naturalSorting();
    static bool opaqueResize();
    static int buttonLayout();

    static QPalette createApplicationPalette();
    static QPalette createApplicationPalette(const KSharedConfigPtr &config);
    static QPalette createNewApplicationPalette();
    static QPalette createNewApplicationPalette(const KSharedConfigPtr &config);

    enum GraphicEffect {
        NoEffects = 0x0000,
        GradientEffects = 0x0001,
        SimpleAnimationEffects = 0x0002,
        ComplexAnimationEffects = 0x0006
    };
    Q_DECLARE_FLAGS(GraphicEffects, GraphicEffect)
    static GraphicEffects graphicEffectsLevel();
    static GraphicEffects graphicEffectsLevelDefault();

    static bool showFilePreview(const KUrl &url);
    static QString desktopPath();
    static QString autostartPath();
    static QString documentPath();
    static QString downloadPath();
    static QString videosPath();
    static QString picturesPath();
    static QString musicPath();

    enum SettingsCategory {
        SETTINGS_MOUSE,
        SETTINGS_COMPLETION,
        SETTINGS_PATHS,
        SETTINGS_POPUPMENU,
        SETTINGS_QT,
        SETTINGS_SHORTCUTS,
        SETTINGS_LOCALE,
        SETTINGS_STYLE
    };

    /* qt4compat: emit the change signals, e.g. after the palette was changed */
    void notifyChange(int changeType, int arg = 0);

Q_SIGNALS:
    void settingsChanged(int category);
    void appearanceChanged();
    void kdisplayPaletteChanged();
    void kdisplayStyleChanged();
    void kdisplayFontChanged();
    void iconChanged(int group);
    void cursorChanged();
    void blockShortcuts(bool block);
    void naturalSortingChanged();

private:
    KGlobalSettings();
    ~KGlobalSettings();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KGlobalSettings::ActivateOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KGlobalSettings::GraphicEffects)

#endif
