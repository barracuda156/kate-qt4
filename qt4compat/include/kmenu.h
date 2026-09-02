/* qt4compat: kmenu.h - QMenu with title items */
#ifndef KDECOMPAT_KMENU_H
#define KDECOMPAT_KMENU_H

#include <QtGui/QMenu>
#include <kdemacros.h>

class KDEUI_EXPORT KMenu : public QMenu
{
    Q_OBJECT
public:
    explicit KMenu(QWidget *parent = 0);
    explicit KMenu(const QString &title, QWidget *parent = 0);
    ~KMenu();

    QAction *addTitle(const QString &text, QAction *before = 0);
    QAction *addTitle(const QIcon &icon, const QString &text, QAction *before = 0);

    void setKeyboardShortcutsEnabled(bool enable);
    void setKeyboardShortcutsExecute(bool enable);
    QMenu *contextMenu();
    const QMenu *contextMenu() const;
    void hideContextMenu();
    static KMenu *contextMenuFocus();
    static QAction *contextMenuFocusAction();
    Qt::MouseButtons mouseButtons() const;
    Qt::KeyboardModifiers keyboardModifiers() const;

Q_SIGNALS:
    void aboutToShowContextMenu(KMenu *menu, QAction *menuAction, QMenu *ctxMenu);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    class KMenuPrivate;
    KMenuPrivate *const d;
};

#endif
