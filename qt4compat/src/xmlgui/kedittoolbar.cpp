/* qt4compat: kedittoolbar.cpp */
#include <kedittoolbar.h>

#include <kactioncollection.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kxmlguiclient.h>
#include <kxmlguifactory.h>

#include <QtGui/QAction>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

class KEditToolBar::Private
{
public:
    Private() : tree(0) {}
    QTreeWidget *tree;
    QString defaultToolBar;
};

static const char *s_globalDefaultToolBar = "mainToolBar";

KEditToolBar::KEditToolBar(KActionCollection *collection, QWidget *parent)
    : QDialog(parent), d(new Private)
{
    init(0, collection);
}

KEditToolBar::KEditToolBar(KXMLGUIFactory *factory, QWidget *parent)
    : QDialog(parent), d(new Private)
{
    init(factory, 0);
}

KEditToolBar::~KEditToolBar()
{
    delete d;
}

void KEditToolBar::init(KXMLGUIFactory *factory, KActionCollection *collection)
{
    setWindowTitle(i18n("Configure Toolbars"));
    setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel(i18n("Toolbar contents are defined by the application. Toolbars can be shown or hidden from the Settings menu, and their icon size and text position from their context menu."), this);
    label->setWordWrap(true);
    layout->addWidget(label);

    d->tree = new QTreeWidget(this);
    d->tree->setColumnCount(1);
    d->tree->setHeaderLabel(i18n("Toolbar"));
    layout->addWidget(d->tree);

    QWidget *window = parentWidget() ? parentWidget()->window() : 0;
    if (factory && qobject_cast<QWidget *>(factory->parent()))
        window = qobject_cast<QWidget *>(factory->parent())->window();
    if (window) {
        foreach (KToolBar *toolbar, window->findChildren<KToolBar *>()) {
            QTreeWidgetItem *top = new QTreeWidgetItem(d->tree, QStringList(toolbar->windowTitle().isEmpty() ? toolbar->objectName() : toolbar->windowTitle()));
            foreach (QAction *action, toolbar->actions()) {
                if (action->isSeparator()) {
                    new QTreeWidgetItem(top, QStringList(QString::fromLatin1("---")));
                    continue;
                }
                QString text = action->text();
                text.remove(QLatin1Char('&'));
                QTreeWidgetItem *item = new QTreeWidgetItem(top, QStringList(text));
                item->setIcon(0, action->icon());
            }
        }
    } else if (collection) {
        QTreeWidgetItem *top = new QTreeWidgetItem(d->tree, QStringList(i18n("Actions")));
        foreach (QAction *action, collection->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            QTreeWidgetItem *item = new QTreeWidgetItem(top, QStringList(text));
            item->setIcon(0, action->icon());
        }
    }
    d->tree->expandAll();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    layout->addWidget(buttons);
    resize(420, 360);
}

void KEditToolBar::setDefaultToolBar(const QString &toolBarName)
{
    d->defaultToolBar = toolBarName;
}

void KEditToolBar::setResourceFile(const QString &, bool)
{
}

void KEditToolBar::setGlobalDefaultToolBar(const char *toolbarName)
{
    s_globalDefaultToolBar = toolbarName;
}
