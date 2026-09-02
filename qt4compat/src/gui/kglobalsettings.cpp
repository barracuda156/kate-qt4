/* qt4compat: kglobalsettings.cpp */
#include <kglobalsettings.h>

#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFontDatabase>
#include <QtGui/QStyle>
#include <QtGui/QWidget>

KGlobalSettings *KGlobalSettings::self()
{
    static KGlobalSettings *s = 0;
    if (!s)
        s = new KGlobalSettings;
    return s;
}

KGlobalSettings::KGlobalSettings()
    : QObject(0)
{
}

KGlobalSettings::~KGlobalSettings()
{
}

void KGlobalSettings::activate()
{
}

void KGlobalSettings::activate(ActivateOptions)
{
}

int KGlobalSettings::dndEventDelay()
{
    return QApplication::startDragDistance();
}

bool KGlobalSettings::singleClick()
{
    return false;
}

bool KGlobalSettings::changeCursorOverIcon()
{
    return true;
}

int KGlobalSettings::autoSelectDelay()
{
    return -1;
}

bool KGlobalSettings::showContextMenusOnPress()
{
    return true;
}

KGlobalSettings::Completion KGlobalSettings::completionMode()
{
    return CompletionPopup;
}

bool KGlobalSettings::insertTearOffHandle()
{
    return false;
}

QColor KGlobalSettings::toolBarHighlightColor()
{
    return QApplication::palette().color(QPalette::Highlight);
}

QColor KGlobalSettings::inactiveTitleColor()
{
    return QColor(224, 223, 222);
}

QColor KGlobalSettings::inactiveTextColor()
{
    return QColor(75, 71, 67);
}

QColor KGlobalSettings::activeTitleColor()
{
    return QApplication::palette().color(QPalette::Highlight);
}

QColor KGlobalSettings::activeTextColor()
{
    return QApplication::palette().color(QPalette::HighlightedText);
}

int KGlobalSettings::contrast()
{
    return 7;
}

qreal KGlobalSettings::contrastF()
{
    return 0.7;
}

qreal KGlobalSettings::contrastF(const KSharedConfigPtr &)
{
    return 0.7;
}

bool KGlobalSettings::shadeSortColumn()
{
    return true;
}

bool KGlobalSettings::allowDefaultBackgroundImages()
{
    return true;
}

QFont KGlobalSettings::generalFont()
{
    return QApplication::font();
}

QFont KGlobalSettings::fixedFont()
{
    static QFont *font = 0;
    if (!font) {
#ifdef Q_OS_MAC
        font = new QFont(QLatin1String("Menlo"), 11);
        if (!QFontDatabase().families().contains(QLatin1String("Menlo")))
            font->setFamily(QLatin1String("Monaco"));
#else
        font = new QFont(QLatin1String("Monospace"), 10);
#endif
        font->setStyleHint(QFont::TypeWriter);
        font->setFixedPitch(true);
    }
    return *font;
}

QFont KGlobalSettings::toolBarFont()
{
    return QApplication::font();
}

QFont KGlobalSettings::menuFont()
{
    return QApplication::font();
}

QFont KGlobalSettings::windowTitleFont()
{
    return QApplication::font();
}

QFont KGlobalSettings::taskbarFont()
{
    return QApplication::font();
}

QFont KGlobalSettings::largeFont(const QString &)
{
    QFont f = QApplication::font();
    f.setPointSize(f.pointSize() + 4);
    return f;
}

QFont KGlobalSettings::smallestReadableFont()
{
    QFont f = QApplication::font();
    if (f.pointSize() > 2)
        f.setPointSize(f.pointSize() - 2);
    return f;
}

bool KGlobalSettings::isMultiHead()
{
    return QApplication::desktop()->screenCount() > 1;
}

bool KGlobalSettings::wheelMouseZooms()
{
    return false;
}

QRect KGlobalSettings::splashScreenDesktopGeometry()
{
    return QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
}

QRect KGlobalSettings::desktopGeometry(const QPoint &point)
{
    return QApplication::desktop()->screenGeometry(point);
}

QRect KGlobalSettings::desktopGeometry(const QWidget *w)
{
    return QApplication::desktop()->screenGeometry(const_cast<QWidget *>(w));
}

bool KGlobalSettings::showIconsOnPushButtons()
{
    return QApplication::style()->styleHint(QStyle::SH_DialogButtonBox_ButtonsHaveIcons) != 0;
}

bool KGlobalSettings::naturalSorting()
{
    return true;
}

bool KGlobalSettings::opaqueResize()
{
    return true;
}

int KGlobalSettings::buttonLayout()
{
    return 0;
}

QPalette KGlobalSettings::createApplicationPalette()
{
    return QApplication::palette();
}

QPalette KGlobalSettings::createApplicationPalette(const KSharedConfigPtr &)
{
    return QApplication::palette();
}

QPalette KGlobalSettings::createNewApplicationPalette()
{
    return QApplication::palette();
}

QPalette KGlobalSettings::createNewApplicationPalette(const KSharedConfigPtr &)
{
    return QApplication::palette();
}

KGlobalSettings::GraphicEffects KGlobalSettings::graphicEffectsLevel()
{
    return SimpleAnimationEffects;
}

KGlobalSettings::GraphicEffects KGlobalSettings::graphicEffectsLevelDefault()
{
    return SimpleAnimationEffects;
}

bool KGlobalSettings::showFilePreview(const KUrl &)
{
    return false;
}

QString KGlobalSettings::desktopPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
}

QString KGlobalSettings::autostartPath()
{
    return QDir::homePath();
}

QString KGlobalSettings::documentPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
}

QString KGlobalSettings::downloadPath()
{
    return QDir::homePath() + QLatin1String("/Downloads");
}

QString KGlobalSettings::videosPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
}

QString KGlobalSettings::picturesPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
}

QString KGlobalSettings::musicPath()
{
    return QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
}

void KGlobalSettings::notifyChange(int changeType, int arg)
{
    switch (changeType) {
    case 0: /* PaletteChanged */
        emit kdisplayPaletteChanged();
        emit appearanceChanged();
        break;
    case 1: /* FontChanged */
        emit kdisplayFontChanged();
        emit appearanceChanged();
        break;
    case 2: /* StyleChanged */
        emit kdisplayStyleChanged();
        emit appearanceChanged();
        break;
    case 3: /* SettingsChanged */
        emit settingsChanged(arg);
        break;
    case 4: /* IconChanged */
        emit iconChanged(arg);
        break;
    case 5: /* CursorChanged */
        emit cursorChanged();
        break;
    default:
        break;
    }
}
