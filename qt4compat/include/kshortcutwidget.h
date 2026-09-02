/* qt4compat: kshortcutwidget.h - primary/alternate shortcut editor built on KKeySequenceWidget */
#ifndef KDECOMPAT_KSHORTCUTWIDGET_H
#define KDECOMPAT_KSHORTCUTWIDGET_H

#include <QtCore/QList>
#include <QtGui/QWidget>
#include <kdemacros.h>
#include <kshortcut.h>

class KActionCollection;
class KKeySequenceWidget;

class KDEUI_EXPORT KShortcutWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(KShortcut shortcut READ shortcut WRITE setShortcut)
public:
    explicit KShortcutWidget(QWidget *parent = 0);
    ~KShortcutWidget();

    void setModifierlessAllowed(bool allow);
    bool isModifierlessAllowed() const;
    void setClearButtonsShown(bool show);
    KShortcut shortcut() const;
    void setCheckActionCollections(const QList<KActionCollection *> &actionCollections);

Q_SIGNALS:
    void shortcutChanged(const KShortcut &cut);

public Q_SLOTS:
    void setShortcut(const KShortcut &cut);
    void clearShortcut();
    void applyStealShortcut();

private Q_SLOTS:
    void priKeySequenceChanged();
    void altKeySequenceChanged();

private:
    KKeySequenceWidget *m_primary;
    KKeySequenceWidget *m_alternate;
    bool m_modifierless;
};

#endif
