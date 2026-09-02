/* qt4compat: ktoolbar.cpp */
#include <ktoolbar.h>

#include <kconfiggroup.h>
#include <kglobal.h>
#include <kicon.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmenu.h>
#include <ksharedconfig.h>
#include <kxmlguiclient.h>

#include <QtCore/QList>
#include <QtGui/QActionGroup>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMainWindow>
#include <QtXml/QDomElement>

static bool s_editable = false;
static bool s_locked = true;

class KToolBar::Private
{
public:
    Private() : isMainToolBar(false), enableContext(true), iconSizeDefault(22) {}
    bool isMainToolBar;
    bool enableContext;
    int iconSizeDefault;
    QList<KXMLGUIClient *> xmlguiClients;
};

static QString toolButtonStyleToString(Qt::ToolButtonStyle style)
{
    switch (style) {
    case Qt::ToolButtonIconOnly: return QLatin1String("IconOnly");
    case Qt::ToolButtonTextBesideIcon: return QLatin1String("TextBesideIcon");
    case Qt::ToolButtonTextOnly: return QLatin1String("TextOnly");
    case Qt::ToolButtonTextUnderIcon: return QLatin1String("TextUnderIcon");
    default: return QLatin1String("IconOnly");
    }
}

static Qt::ToolButtonStyle toolButtonStyleFromString(const QString &_style)
{
    const QString style = _style.toLower();
    if (style == QLatin1String("textbesideicon") || style == QLatin1String("icontextright"))
        return Qt::ToolButtonTextBesideIcon;
    if (style == QLatin1String("textundericon") || style == QLatin1String("icontextbottom"))
        return Qt::ToolButtonTextUnderIcon;
    if (style == QLatin1String("textonly"))
        return Qt::ToolButtonTextOnly;
    return Qt::ToolButtonIconOnly;
}

KToolBar::KToolBar(QWidget *parent, bool isMainToolBar, bool readConfig)
    : QToolBar(parent), d(new Private)
{
    init(readConfig, isMainToolBar);
}

KToolBar::KToolBar(const QString &objectName, QWidget *parent, bool readConfig)
    : QToolBar(parent), d(new Private)
{
    setObjectName(objectName);
    init(readConfig, objectName == QLatin1String("mainToolBar"));
}

KToolBar::KToolBar(const QString &objectName, QMainWindow *parent, Qt::ToolBarArea area, bool newLine, bool isMainToolBar, bool readConfig)
    : QToolBar(parent), d(new Private)
{
    setObjectName(objectName);
    init(readConfig, isMainToolBar);
    if (newLine)
        parent->addToolBarBreak(area);
    parent->addToolBar(area, this);
    if (newLine)
        parent->addToolBarBreak(area);
}

KToolBar::~KToolBar()
{
    delete d;
}

void KToolBar::init(bool readConfig, bool isMainToolBar)
{
    d->isMainToolBar = isMainToolBar;
    setWindowTitle(isMainToolBar ? i18n("Main Toolbar") : i18n("Toolbar"));
    setMovable(!toolBarsLocked());
    setFloatable(false);
    setIconSize(QSize(d->iconSizeDefault, d->iconSizeDefault));
    setToolButtonStyle(isMainToolBar ? toolButtonStyleSetting() : Qt::ToolButtonIconOnly);
    connect(this, SIGNAL(movableChanged(bool)), this, SLOT(slotMovableChanged(bool)));
    if (QMainWindow *mw = qobject_cast<QMainWindow *>(parentWidget()))
        mw->addToolBar(this);
    if (readConfig) {
        KConfigGroup cg(KGlobal::config(), QString());
        applySettings(cg);
    }
}

KMainWindow *KToolBar::mainWindow() const
{
    return qobject_cast<KMainWindow *>(const_cast<QObject *>(parent()));
}

void KToolBar::setIconDimensions(int size)
{
    setIconSize(QSize(size, size));
}

int KToolBar::iconSizeDefault() const
{
    return d->iconSizeDefault;
}

void KToolBar::setContextMenuEnabled(bool enable)
{
    d->enableContext = enable;
}

bool KToolBar::contextMenuEnabled() const
{
    return d->enableContext;
}

void KToolBar::saveSettings(KConfigGroup &cg)
{
    const Qt::ToolButtonStyle defaultStyle = d->isMainToolBar ? toolButtonStyleSetting() : Qt::ToolButtonIconOnly;
    if (toolButtonStyle() == defaultStyle)
        cg.deleteEntry("ToolButtonStyle");
    else
        cg.writeEntry("ToolButtonStyle", toolButtonStyleToString(toolButtonStyle()));
    if (iconSize().width() == d->iconSizeDefault)
        cg.deleteEntry("IconSize");
    else
        cg.writeEntry("IconSize", iconSize().width());
}

void KToolBar::applySettings(const KConfigGroup &cg, bool)
{
    if (cg.hasKey("ToolButtonStyle"))
        setToolButtonStyle(toolButtonStyleFromString(cg.readEntry("ToolButtonStyle", QString())));
    if (cg.hasKey("IconSize")) {
        const int size = cg.readEntry("IconSize", d->iconSizeDefault);
        if (size > 0)
            setIconDimensions(size);
    }
}

void KToolBar::setXMLGUIClient(KXMLGUIClient *client)
{
    d->xmlguiClients.clear();
    d->xmlguiClients << client;
}

void KToolBar::addXMLGUIClient(KXMLGUIClient *client)
{
    if (!d->xmlguiClients.contains(client))
        d->xmlguiClients << client;
}

void KToolBar::removeXMLGUIClient(KXMLGUIClient *client)
{
    d->xmlguiClients.removeAll(client);
}

void KToolBar::loadState(const QDomElement &element)
{
    QMainWindow *mw = qobject_cast<QMainWindow *>(parentWidget());
    if (!mw)
        return;

    {
        QDomNode textNode = element.namedItem(QLatin1String("text"));
        if (textNode.isNull())
            textNode = element.namedItem(QLatin1String("Text"));
        const QDomElement textElement = textNode.toElement();
        const QByteArray text = textElement.text().toUtf8();
        const QByteArray context = textElement.attribute(QLatin1String("context")).toUtf8();
        QString i18nText;
        if (!text.isEmpty()) {
            if (context.isEmpty())
                i18nText = i18n(text.constData());
            else
                i18nText = i18nc(context.constData(), text.constData());
            setWindowTitle(i18nText);
        }
    }

    const QString attrIconText = element.attribute(QLatin1String("iconText")).toLower();
    if (!attrIconText.isEmpty())
        setToolButtonStyle(toolButtonStyleFromString(attrIconText));

    const QString attrIconSize = element.attribute(QLatin1String("iconSize")).trimmed();
    if (!attrIconSize.isEmpty()) {
        bool ok = false;
        const int size = attrIconSize.toInt(&ok);
        if (ok && size > 0)
            setIconDimensions(size);
    }

    const QString attrPosition = element.attribute(QLatin1String("position")).toLower();
    if (!attrPosition.isEmpty()) {
        Qt::ToolBarArea area = Qt::TopToolBarArea;
        if (attrPosition == QLatin1String("left"))
            area = Qt::LeftToolBarArea;
        else if (attrPosition == QLatin1String("right"))
            area = Qt::RightToolBarArea;
        else if (attrPosition == QLatin1String("bottom"))
            area = Qt::BottomToolBarArea;
        mw->addToolBar(area, this);
    }

    const QString attrNewLine = element.attribute(QLatin1String("newline")).toLower();
    if (attrNewLine == QLatin1String("true"))
        mw->insertToolBarBreak(this);

    const QString attrHidden = element.attribute(QLatin1String("hidden")).toLower();
    if (attrHidden == QLatin1String("true"))
        hide();
    else
        show();
}

void KToolBar::saveState(QDomElement &current) const
{
    current.setAttribute(QLatin1String("noMerge"), QLatin1String("1"));
    QMainWindow *mw = qobject_cast<QMainWindow *>(parentWidget());
    QString position = QLatin1String("top");
    if (mw) {
        switch (mw->toolBarArea(const_cast<KToolBar *>(this))) {
        case Qt::LeftToolBarArea: position = QLatin1String("left"); break;
        case Qt::RightToolBarArea: position = QLatin1String("right"); break;
        case Qt::BottomToolBarArea: position = QLatin1String("bottom"); break;
        default: break;
        }
    }
    current.setAttribute(QLatin1String("position"), position);
    current.setAttribute(QLatin1String("iconText"), toolButtonStyleToString(toolButtonStyle()));
    current.setAttribute(QLatin1String("iconSize"), iconSize().width());
    current.setAttribute(QLatin1String("hidden"), isHidden() ? QLatin1String("true") : QLatin1String("false"));
}

bool KToolBar::eventFilter(QObject *watched, QEvent *event)
{
    return QToolBar::eventFilter(watched, event);
}

Qt::ToolButtonStyle KToolBar::toolButtonStyleSetting()
{
    KConfigGroup group(KGlobal::config(), "Toolbar style");
    return toolButtonStyleFromString(group.readEntry("ToolButtonStyle", QString::fromLatin1("IconOnly")));
}

bool KToolBar::toolBarsEditable()
{
    return s_editable;
}

void KToolBar::setToolBarsEditable(bool editable)
{
    s_editable = editable;
}

bool KToolBar::toolBarsLocked()
{
    return s_locked;
}

void KToolBar::setToolBarsLocked(bool locked)
{
    if (s_locked == locked)
        return;
    s_locked = locked;
    foreach (KMainWindow *mw, KMainWindow::memberList()) {
        foreach (KToolBar *toolbar, mw->toolBars())
            toolbar->setMovable(!locked);
    }
}

void KToolBar::slotMovableChanged(bool movable)
{
    if (movable == s_locked)
        setToolBarsLocked(!movable);
}

void KToolBar::contextMenuEvent(QContextMenuEvent *event)
{
    if (!d->enableContext) {
        QToolBar::contextMenuEvent(event);
        return;
    }
    KMenu menu(this);
    menu.addTitle(i18n("Toolbar Settings"));

    KMenu *textMenu = new KMenu(i18n("Text Position"), &menu);
    QActionGroup *textGroup = new QActionGroup(textMenu);
    struct { const char *text; Qt::ToolButtonStyle style; } textModes[] = {
        { I18N_NOOP("Icons Only"), Qt::ToolButtonIconOnly },
        { I18N_NOOP("Text Only"), Qt::ToolButtonTextOnly },
        { I18N_NOOP("Text Alongside Icons"), Qt::ToolButtonTextBesideIcon },
        { I18N_NOOP("Text Under Icons"), Qt::ToolButtonTextUnderIcon }
    };
    for (uint i = 0; i < sizeof(textModes) / sizeof(textModes[0]); ++i) {
        QAction *a = textMenu->addAction(i18n(textModes[i].text));
        a->setCheckable(true);
        a->setData(int(textModes[i].style));
        a->setChecked(toolButtonStyle() == textModes[i].style);
        textGroup->addAction(a);
    }
    connect(textGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotContextTextPosition(QAction*)));
    menu.addMenu(textMenu);

    KMenu *sizeMenu = new KMenu(i18n("Icon Size"), &menu);
    QActionGroup *sizeGroup = new QActionGroup(sizeMenu);
    const int sizes[] = { 16, 22, 32, 48 };
    for (uint i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        QString text;
        if (sizes[i] == d->iconSizeDefault)
            text = i18nc("@item:inmenu Icon size", "Default (%1x%2)", sizes[i], sizes[i]);
        else
            text = i18nc("@item:inmenu Icon size", "%1x%2", sizes[i], sizes[i]);
        QAction *a = sizeMenu->addAction(text);
        a->setCheckable(true);
        a->setData(sizes[i]);
        a->setChecked(iconSize().width() == sizes[i]);
        sizeGroup->addAction(a);
    }
    connect(sizeGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotContextIconSize(QAction*)));
    menu.addMenu(sizeMenu);

    menu.addSeparator();
    QAction *lock = menu.addAction(KIcon(QLatin1String("system-lock-screen")), i18n("Lock Toolbar Positions"));
    lock->setCheckable(true);
    lock->setChecked(toolBarsLocked());
    connect(lock, SIGNAL(toggled(bool)), this, SLOT(slotLockToolBars(bool)));

    menu.exec(event->globalPos());
}

void KToolBar::slotContextTextPosition(QAction *action)
{
    setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(action->data().toInt()));
    if (mainWindow())
        mainWindow()->setSettingsDirty();
}

void KToolBar::slotContextIconSize(QAction *action)
{
    setIconDimensions(action->data().toInt());
    if (mainWindow())
        mainWindow()->setSettingsDirty();
}

void KToolBar::slotLockToolBars(bool lock)
{
    setToolBarsLocked(lock);
    if (mainWindow())
        mainWindow()->setSettingsDirty();
}
