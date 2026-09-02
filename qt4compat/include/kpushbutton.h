/* qt4compat: kpushbutton.h - KPushButton without KAuth and drag support */
#ifndef KDECOMPAT_KPUSHBUTTON_H
#define KDECOMPAT_KPUSHBUTTON_H

#include <QtGui/QPushButton>
#include <kdemacros.h>
#include <kguiitem.h>
#include <kstandardguiitem.h>

class QMenu;
class KIcon;

class KDEUI_EXPORT KPushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(bool isDragEnabled READ isDragEnabled WRITE setDragEnabled)
public:
    explicit KPushButton(QWidget *parent = 0);
    explicit KPushButton(const QString &text, QWidget *parent = 0);
    KPushButton(const KIcon &icon, const QString &text, QWidget *parent = 0);
    explicit KPushButton(const KGuiItem &item, QWidget *parent = 0);
    ~KPushButton();

    void setDragEnabled(bool enable);
    bool isDragEnabled() const;
    void setGuiItem(const KGuiItem &item);
    void setGuiItem(KStandardGuiItem::StandardItem item);
    KStandardGuiItem::StandardItem guiItem() const;
    void setIcon(const KIcon &icon);
    void setIcon(const QIcon &pix);
    void setText(const QString &text);
    void setDelayedMenu(QMenu *delayed_menu);
    QMenu *delayedMenu();
    virtual QSize sizeHint() const;

private:
    void init(const KGuiItem &item);
    bool m_dragEnabled;
    KStandardGuiItem::StandardItem m_itemType;
    QMenu *m_delayedMenu;
};

#endif
