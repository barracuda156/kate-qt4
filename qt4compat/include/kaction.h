/* qt4compat: kaction.h - KAction on QWidgetAction (kdelibs 4.14 API, no global shortcuts) */
#ifndef KDECOMPAT_KACTION_H
#define KDECOMPAT_KACTION_H

#include <QtGui/QWidgetAction>
#include <kdemacros.h>
#include <kshortcut.h>

class KIcon;
class KActionPrivate;

class KDEUI_EXPORT KAction : public QWidgetAction
{
    Q_OBJECT
    Q_PROPERTY(KShortcut shortcut READ shortcut WRITE setShortcut)
    Q_PROPERTY(bool shortcutConfigurable READ isShortcutConfigurable WRITE setShortcutConfigurable)
    Q_PROPERTY(KShortcut globalShortcut READ globalShortcut WRITE setGlobalShortcut)
    Q_FLAGS(ShortcutType)
public:
    enum ShortcutType {
        ActiveShortcut = 0x1,
        DefaultShortcut = 0x2
    };
    Q_DECLARE_FLAGS(ShortcutTypes, ShortcutType)

    enum GlobalShortcutLoading {
        Autoloading = 0x0,
        NoAutoloading = 0x4
    };

    explicit KAction(QObject *parent);
    KAction(const QString &text, QObject *parent);
    KAction(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KAction();

    void setHelpText(const QString &text);

    KShortcut shortcut(ShortcutTypes types = ActiveShortcut) const;
    void setShortcut(const KShortcut &shortcut, ShortcutTypes type = ShortcutTypes(ActiveShortcut | DefaultShortcut));
    void setShortcut(const QKeySequence &shortcut, ShortcutTypes type = ShortcutTypes(ActiveShortcut | DefaultShortcut));
    void setShortcuts(const QList<QKeySequence> &shortcuts, ShortcutTypes type = ShortcutTypes(ActiveShortcut | DefaultShortcut));

    bool isShortcutConfigurable() const;
    void setShortcutConfigurable(bool configurable);

    const KShortcut &globalShortcut(ShortcutTypes type = ActiveShortcut) const;
    void setGlobalShortcut(const KShortcut &shortcut, ShortcutTypes type = ShortcutTypes(ActiveShortcut | DefaultShortcut),
                           GlobalShortcutLoading loading = Autoloading);
    bool globalShortcutAllowed() const;
    void setGlobalShortcutAllowed(bool allowed, GlobalShortcutLoading loading = Autoloading);
    bool isGlobalShortcutEnabled() const;
    void forgetGlobalShortcut();

    virtual bool event(QEvent *);

Q_SIGNALS:
    void triggered(Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void globalShortcutChanged(const QKeySequence &);

private Q_SLOTS:
    void slotTriggered();

private:
    void init();
    KActionPrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KAction::ShortcutTypes)

#endif
