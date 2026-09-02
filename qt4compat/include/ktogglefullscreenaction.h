/* qt4compat: ktogglefullscreenaction.h */
#ifndef KDECOMPAT_KTOGGLEFULLSCREENACTION_H
#define KDECOMPAT_KTOGGLEFULLSCREENACTION_H

#include <ktoggleaction.h>

class KDEUI_EXPORT KToggleFullScreenAction : public KToggleAction
{
    Q_OBJECT
public:
    explicit KToggleFullScreenAction(QObject *parent);
    KToggleFullScreenAction(QWidget *window, QObject *parent);
    virtual ~KToggleFullScreenAction();

    void setWindow(QWidget *window);
    static void setFullScreen(QWidget *window, bool set);

protected:
    bool eventFilter(QObject *object, QEvent *event);

protected Q_SLOTS:
    virtual void slotToggled(bool checked);

private:
    void init();
    class Private;
    Private *const d;
};

#endif
