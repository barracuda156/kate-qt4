/* qt4compat: ktoggleaction.cpp */
#include <ktoggleaction.h>
#include <kguiitem.h>
#include <kicon.h>

class KToggleAction::Private
{
public:
    Private() : hasCheckedItem(false), hasIcon(false) {}
    bool hasCheckedItem;
    QString text;
    QString toolTip;
    QIcon icon;
    bool hasIcon;
};

KToggleAction::KToggleAction(QObject *parent)
    : KAction(parent), d(new Private)
{
    init();
}

KToggleAction::KToggleAction(const QString &text, QObject *parent)
    : KAction(text, parent), d(new Private)
{
    init();
}

KToggleAction::KToggleAction(const KIcon &icon, const QString &text, QObject *parent)
    : KAction(icon, text, parent), d(new Private)
{
    init();
}

KToggleAction::~KToggleAction()
{
    delete d;
}

void KToggleAction::init()
{
    setCheckable(true);
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
}

void KToggleAction::setCheckedState(const KGuiItem &checkedItem)
{
    d->hasCheckedItem = true;
    d->text = checkedItem.text();
    d->toolTip = checkedItem.toolTip();
    d->hasIcon = checkedItem.hasIcon();
    d->icon = checkedItem.icon();
}

void KToggleAction::slotToggled(bool)
{
    if (!d->hasCheckedItem)
        return;
    /* swap the two states, like kdelibs does */
    QString string = d->text;
    d->text = text();
    setText(string);
    string = d->toolTip;
    d->toolTip = toolTip();
    setToolTip(string);
    if (d->hasIcon) {
        QIcon i = d->icon;
        d->icon = icon();
        setIcon(i);
    }
}
