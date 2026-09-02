/* qt4compat: kpastetextaction.h - plain paste action (no clipboard history) */
#ifndef KDECOMPAT_KPASTETEXTACTION_H
#define KDECOMPAT_KPASTETEXTACTION_H

#include <kaction.h>

class KDEUI_EXPORT KPasteTextAction : public KAction
{
    Q_OBJECT
public:
    explicit KPasteTextAction(QObject *parent);
    KPasteTextAction(const QString &text, QObject *parent);
    KPasteTextAction(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KPasteTextAction();

    void setMixedMode(bool mode);

private:
    void init();
    bool m_mixedMode;
};

#endif
