/* qt4compat: kshortcutwidget.cpp */
#include <kshortcutwidget.h>
#include <kkeysequencewidget.h>
#include <klocale.h>

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

KShortcutWidget::KShortcutWidget(QWidget *parent)
    : QWidget(parent), m_modifierless(false)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    QLabel *priLabel = new QLabel(i18n("Main:"), this);
    m_primary = new KKeySequenceWidget(this);
    QLabel *altLabel = new QLabel(i18n("Alternate:"), this);
    m_alternate = new KKeySequenceWidget(this);
    layout->addWidget(priLabel);
    layout->addWidget(m_primary);
    layout->addWidget(altLabel);
    layout->addWidget(m_alternate);
    layout->addStretch(1);
    connect(m_primary, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(priKeySequenceChanged()));
    connect(m_alternate, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(altKeySequenceChanged()));
}

KShortcutWidget::~KShortcutWidget()
{
}

void KShortcutWidget::setModifierlessAllowed(bool allow)
{
    m_modifierless = allow;
    m_primary->setModifierlessAllowed(allow);
    m_alternate->setModifierlessAllowed(allow);
}

bool KShortcutWidget::isModifierlessAllowed() const
{
    return m_modifierless;
}

void KShortcutWidget::setClearButtonsShown(bool show)
{
    m_primary->setClearButtonShown(show);
    m_alternate->setClearButtonShown(show);
}

KShortcut KShortcutWidget::shortcut() const
{
    return KShortcut(m_primary->keySequence(), m_alternate->keySequence());
}

void KShortcutWidget::setCheckActionCollections(const QList<KActionCollection *> &actionCollections)
{
    m_primary->setCheckActionCollections(actionCollections);
    m_alternate->setCheckActionCollections(actionCollections);
}

void KShortcutWidget::setShortcut(const KShortcut &cut)
{
    if (cut == shortcut())
        return;
    m_primary->setKeySequence(cut.primary(), KKeySequenceWidget::NoValidate);
    m_alternate->setKeySequence(cut.alternate(), KKeySequenceWidget::NoValidate);
    emit shortcutChanged(cut);
}

void KShortcutWidget::clearShortcut()
{
    setShortcut(KShortcut());
}

void KShortcutWidget::applyStealShortcut()
{
    m_primary->applyStealShortcut();
    m_alternate->applyStealShortcut();
}

void KShortcutWidget::priKeySequenceChanged()
{
    emit shortcutChanged(shortcut());
}

void KShortcutWidget::altKeySequenceChanged()
{
    emit shortcutChanged(shortcut());
}
