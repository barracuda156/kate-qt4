/* qt4compat: kaction.cpp */
#include <kaction.h>
#include <kicon.h>

#include <QtGui/QApplication>

class KActionPrivate
{
public:
    KActionPrivate() : configurable(true) {}
    KShortcut defaultShortcut;
    KShortcut globalShortcut;
    bool configurable;
};

KAction::KAction(QObject *parent)
    : QWidgetAction(parent), d(new KActionPrivate)
{
    init();
}

KAction::KAction(const QString &text, QObject *parent)
    : QWidgetAction(parent), d(new KActionPrivate)
{
    init();
    setText(text);
}

KAction::KAction(const KIcon &icon, const QString &text, QObject *parent)
    : QWidgetAction(parent), d(new KActionPrivate)
{
    init();
    setIcon(icon);
    setText(text);
}

KAction::~KAction()
{
    delete d;
}

void KAction::init()
{
    setProperty("isShortcutConfigurable", true);
    connect(this, SIGNAL(triggered(bool)), this, SLOT(slotTriggered()));
}

void KAction::setHelpText(const QString &text)
{
    setStatusTip(text);
    setToolTip(text);
    if (whatsThis().isEmpty())
        setWhatsThis(text);
}

KShortcut KAction::shortcut(ShortcutTypes types) const
{
    if (types & DefaultShortcut)
        return d->defaultShortcut;
    return KShortcut(shortcuts());
}

void KAction::setShortcut(const KShortcut &shortcut, ShortcutTypes type)
{
    if (type & DefaultShortcut)
        d->defaultShortcut = shortcut;
    if (type & ActiveShortcut)
        QAction::setShortcuts(shortcut.toList());
}

void KAction::setShortcut(const QKeySequence &keySeq, ShortcutTypes type)
{
    if (type & DefaultShortcut)
        d->defaultShortcut.setPrimary(keySeq);
    if (type & ActiveShortcut)
        QAction::setShortcut(keySeq);
}

void KAction::setShortcuts(const QList<QKeySequence> &shortcuts, ShortcutTypes type)
{
    setShortcut(KShortcut(shortcuts), type);
}

bool KAction::isShortcutConfigurable() const
{
    return d->configurable;
}

void KAction::setShortcutConfigurable(bool configurable)
{
    d->configurable = configurable;
    setProperty("isShortcutConfigurable", configurable);
}

const KShortcut &KAction::globalShortcut(ShortcutTypes) const
{
    return d->globalShortcut;
}

void KAction::setGlobalShortcut(const KShortcut &shortcut, ShortcutTypes, GlobalShortcutLoading)
{
    /* no global shortcut support without a desktop daemon; remember it only */
    d->globalShortcut = shortcut;
}

bool KAction::globalShortcutAllowed() const
{
    return false;
}

void KAction::setGlobalShortcutAllowed(bool, GlobalShortcutLoading)
{
}

bool KAction::isGlobalShortcutEnabled() const
{
    return false;
}

void KAction::forgetGlobalShortcut()
{
    d->globalShortcut = KShortcut();
}

bool KAction::event(QEvent *e)
{
    return QWidgetAction::event(e);
}

void KAction::slotTriggered()
{
    emit triggered(QApplication::mouseButtons(), QApplication::keyboardModifiers());
}
