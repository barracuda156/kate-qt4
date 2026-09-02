/* qt4compat: kedittoolbar.h - read-only toolbar overview (no drag-and-drop editing yet) */
#ifndef KDECOMPAT_KEDITTOOLBAR_H
#define KDECOMPAT_KEDITTOOLBAR_H

#include <QtGui/QDialog>
#include <kdemacros.h>

class KActionCollection;
class KXMLGUIFactory;

class KDEUI_EXPORT KEditToolBar : public QDialog
{
    Q_OBJECT
public:
    explicit KEditToolBar(KActionCollection *collection, QWidget *parent = 0);
    explicit KEditToolBar(KXMLGUIFactory *factory, QWidget *parent = 0);
    ~KEditToolBar();

    void setDefaultToolBar(const QString &toolBarName);
    void setResourceFile(const QString &file, bool global = true);
    static void setGlobalDefaultToolBar(const char *toolbarName);

Q_SIGNALS:
    void newToolBarConfig();
    void newToolbarConfig();

private:
    void init(KXMLGUIFactory *factory, KActionCollection *collection);
    class Private;
    Private *const d;
};

#endif
