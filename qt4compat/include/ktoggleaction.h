/* qt4compat: ktoggleaction.h */
#ifndef KDECOMPAT_KTOGGLEACTION_H
#define KDECOMPAT_KTOGGLEACTION_H

#include <kaction.h>

class KGuiItem;

class KDEUI_EXPORT KToggleAction : public KAction
{
    Q_OBJECT
public:
    explicit KToggleAction(QObject *parent);
    KToggleAction(const QString &text, QObject *parent);
    KToggleAction(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KToggleAction();

    void setCheckedState(const KGuiItem &checkedItem);

protected Q_SLOTS:
    virtual void slotToggled(bool checked);

private:
    void init();
    class Private;
    Private *const d;
};

#endif
