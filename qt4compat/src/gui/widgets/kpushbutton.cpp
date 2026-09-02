/* qt4compat: kpushbutton.cpp */
#include <kpushbutton.h>

#include <kglobalsettings.h>
#include <kicon.h>

#include <QtGui/QMenu>

KPushButton::KPushButton(QWidget *parent)
    : QPushButton(parent), m_dragEnabled(false), m_itemType(static_cast<KStandardGuiItem::StandardItem>(0)), m_delayedMenu(0)
{
    init(KGuiItem(QString()));
}

KPushButton::KPushButton(const QString &text, QWidget *parent)
    : QPushButton(parent), m_dragEnabled(false), m_itemType(static_cast<KStandardGuiItem::StandardItem>(0)), m_delayedMenu(0)
{
    init(KGuiItem(text));
}

KPushButton::KPushButton(const KIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(parent), m_dragEnabled(false), m_itemType(static_cast<KStandardGuiItem::StandardItem>(0)), m_delayedMenu(0)
{
    init(KGuiItem(text, icon));
}

KPushButton::KPushButton(const KGuiItem &item, QWidget *parent)
    : QPushButton(parent), m_dragEnabled(false), m_itemType(static_cast<KStandardGuiItem::StandardItem>(0)), m_delayedMenu(0)
{
    init(item);
}

KPushButton::~KPushButton()
{
}

void KPushButton::init(const KGuiItem &item)
{
    setGuiItem(item);
}

void KPushButton::setDragEnabled(bool enable)
{
    m_dragEnabled = enable;
}

bool KPushButton::isDragEnabled() const
{
    return m_dragEnabled;
}

void KPushButton::setGuiItem(const KGuiItem &item)
{
    QPushButton::setText(item.text());
    if (item.hasIcon() && KGlobalSettings::showIconsOnPushButtons())
        QPushButton::setIcon(item.icon());
    else
        QPushButton::setIcon(QIcon());
    setToolTip(item.toolTip());
    setWhatsThis(item.whatsThis());
}

void KPushButton::setGuiItem(KStandardGuiItem::StandardItem item)
{
    setGuiItem(KStandardGuiItem::guiItem(item));
    m_itemType = item;
}

KStandardGuiItem::StandardItem KPushButton::guiItem() const
{
    return m_itemType;
}

void KPushButton::setIcon(const KIcon &icon)
{
    if (KGlobalSettings::showIconsOnPushButtons())
        QPushButton::setIcon(icon);
    else
        QPushButton::setIcon(QIcon());
}

void KPushButton::setIcon(const QIcon &icon)
{
    QPushButton::setIcon(icon);
}

void KPushButton::setText(const QString &text)
{
    QPushButton::setText(text);
}

void KPushButton::setDelayedMenu(QMenu *delayed_menu)
{
    m_delayedMenu = delayed_menu;
    setMenu(delayed_menu);
}

QMenu *KPushButton::delayedMenu()
{
    return m_delayedMenu;
}

QSize KPushButton::sizeHint() const
{
    return QPushButton::sizeHint();
}
