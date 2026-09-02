/* qt4compat: kpastetextaction.cpp */
#include <kpastetextaction.h>
#include <kicon.h>
#include <klocale.h>
#include <kstandardshortcut.h>

KPasteTextAction::KPasteTextAction(QObject *parent)
    : KAction(parent), m_mixedMode(true)
{
    init();
}

KPasteTextAction::KPasteTextAction(const QString &text, QObject *parent)
    : KAction(text, parent), m_mixedMode(true)
{
    init();
}

KPasteTextAction::KPasteTextAction(const KIcon &icon, const QString &text, QObject *parent)
    : KAction(icon, text, parent), m_mixedMode(true)
{
    init();
}

KPasteTextAction::~KPasteTextAction()
{
}

void KPasteTextAction::init()
{
    if (text().isEmpty())
        setText(i18n("&Paste"));
    if (icon().isNull())
        setIcon(KIcon(QLatin1String("edit-paste")));
    setShortcut(KStandardShortcut::paste());
}

void KPasteTextAction::setMixedMode(bool mode)
{
    m_mixedMode = mode;
}
