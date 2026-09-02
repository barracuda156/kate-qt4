/* qt4compat: ktoggletoolbaraction.h */
#ifndef KDECOMPAT_KTOGGLETOOLBARACTION_H
#define KDECOMPAT_KTOGGLETOOLBARACTION_H

#include <ktoggleaction.h>

class KToolBar;

class KDEUI_EXPORT KToggleToolBarAction : public KToggleAction
{
    Q_OBJECT
public:
    KToggleToolBarAction(const char *toolBarName, const QString &text, QObject *parent);
    KToggleToolBarAction(KToolBar *toolBar, const QString &text, QObject *parent);
    virtual ~KToggleToolBarAction();

    KToolBar *toolBar();
    virtual bool eventFilter(QObject *watched, QEvent *event);

protected Q_SLOTS:
    virtual void slotToggled(bool checked);

private:
    class Private;
    Private *const d;
};

#endif
