/* qt4compat: khelpmenu.h - minimal Help menu (about box via QMessageBox, help via KToolInvocation) */
#ifndef KDECOMPAT_KHELPMENU_H
#define KDECOMPAT_KHELPMENU_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <kdemacros.h>

class QAction;
class QWidget;
class KAboutData;
class KActionCollection;
class KMenu;

class KDEUI_EXPORT KHelpMenu : public QObject
{
    Q_OBJECT
public:
    explicit KHelpMenu(QWidget *parent = 0, const QString &aboutAppText = QString(), bool showWhatsThis = true);
    KHelpMenu(QWidget *parent, const KAboutData *aboutData, bool showWhatsThis = true, KActionCollection *actions = 0);
    ~KHelpMenu();

    KMenu *menu();

    enum MenuId {
        menuHelpContents = 0,
        menuWhatsThis = 1,
        menuAboutApp = 2,
        menuAboutKDE = 3,
        menuReportBug = 4,
        menuSwitchLanguage = 5
    };

    QAction *action(MenuId id) const;

public Q_SLOTS:
    void appHelpActivated();
    void contextHelpActivated();
    void aboutApplication();
    void aboutKDE();
    void reportBug();
    void switchApplicationLanguage();

Q_SIGNALS:
    void showAboutApplication();

private:
    void init(QWidget *parent, bool showWhatsThis, KActionCollection *actions);
    class Private;
    Private *const d;
};

#endif
