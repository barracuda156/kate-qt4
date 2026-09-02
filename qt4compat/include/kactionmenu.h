/* qt4compat: kactionmenu.h */
#ifndef KDECOMPAT_KACTIONMENU_H
#define KDECOMPAT_KACTIONMENU_H

#include <kaction.h>

class KMenu;

class KDEUI_EXPORT KActionMenu : public KAction
{
    Q_OBJECT
    Q_PROPERTY(bool delayed READ delayed WRITE setDelayed)
    Q_PROPERTY(bool stickyMenu READ stickyMenu WRITE setStickyMenu)
public:
    explicit KActionMenu(QObject *parent);
    KActionMenu(const QString &text, QObject *parent);
    KActionMenu(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KActionMenu();

    void remove(KAction *);
    void addAction(QAction *action);
    QAction *addSeparator();
    void insertAction(QAction *before, QAction *action);
    QAction *insertSeparator(QAction *before);
    void removeAction(QAction *action);
    inline KMenu *popupMenu() { return menu(); }

    KMenu *menu();
    void setMenu(KMenu *menu);

    bool delayed() const;
    void setDelayed(bool delayed);
    bool stickyMenu() const;
    void setStickyMenu(bool sticky);

    virtual QWidget *createWidget(QWidget *parent);

private:
    class KActionMenuPrivate *const d;
};

#endif
