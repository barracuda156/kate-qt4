/* qt4compat: kmimetypechooser.cpp */
#include <kmimetypechooser.h>

#include <kconfiggroup.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmimetype.h>
#include <ksharedconfig.h>

#include <QtCore/QMap>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTreeWidget>

KMimeTypeChooser::KMimeTypeChooser(const QString &text, const QStringList &selectedMimeTypes, const QString &defaultGroup,
                                   const QStringList &groupsToShow, int visuals, QWidget *parent)
    : KVBox(parent), m_tree(0)
{
    setSpacing(-1);
    if (!text.isEmpty()) {
        QLabel *l = new QLabel(text, this);
        l->setWordWrap(true);
    }
    m_tree = new QTreeWidget(this);
    QStringList headers;
    headers << i18n("Mime Type");
    if (visuals & Comments)
        headers << i18n("Comment");
    if (visuals & Patterns)
        headers << i18n("Patterns");
    m_tree->setColumnCount(headers.count());
    m_tree->setHeaderLabels(headers);
    m_tree->setRootIsDecorated(true);
    m_tree->setAllColumnsShowFocus(true);
    loadMimeTypes(selectedMimeTypes, defaultGroup, groupsToShow, visuals);
}

KMimeTypeChooser::~KMimeTypeChooser()
{
}

void KMimeTypeChooser::loadMimeTypes(const QStringList &selected, const QString &defaultGroup, const QStringList &groupsToShow, int visuals)
{
    m_tree->clear();
    QMap<QString, QTreeWidgetItem *> groups;
    QTreeWidgetItem *defaultGroupItem = 0;
    const KMimeType::List mimetypes = KMimeType::allMimeTypes();
    for (KMimeType::List::ConstIterator it = mimetypes.begin(); it != mimetypes.end(); ++it) {
        const QString name = (*it)->name();
        const int slash = name.indexOf(QLatin1Char('/'));
        if (slash <= 0)
            continue;
        const QString major = name.left(slash);
        const QString minor = name.mid(slash + 1);
        if (!groupsToShow.isEmpty() && !groupsToShow.contains(major))
            continue;
        QTreeWidgetItem *groupItem = groups.value(major);
        if (!groupItem) {
            groupItem = new QTreeWidgetItem(m_tree, QStringList(major));
            groups.insert(major, groupItem);
            if (major == defaultGroup)
                defaultGroupItem = groupItem;
        }
        QStringList columns;
        columns << minor;
        if (visuals & Comments)
            columns << (*it)->comment();
        if (visuals & Patterns)
            columns << (*it)->patterns().join(QLatin1String("; "));
        QTreeWidgetItem *item = new QTreeWidgetItem(groupItem, columns);
        item->setData(0, Qt::UserRole, name);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, selected.contains(name) ? Qt::Checked : Qt::Unchecked);
        if (selected.contains(name))
            groupItem->setExpanded(true);
    }
    m_tree->sortItems(0, Qt::AscendingOrder);
    if (defaultGroupItem) {
        defaultGroupItem->setExpanded(true);
        m_tree->scrollToItem(defaultGroupItem);
    }
    for (int i = 0; i < m_tree->columnCount(); ++i)
        m_tree->resizeColumnToContents(i);
}

QStringList KMimeTypeChooser::mimeTypes() const
{
    QStringList result;
    for (int g = 0; g < m_tree->topLevelItemCount(); ++g) {
        QTreeWidgetItem *group = m_tree->topLevelItem(g);
        for (int i = 0; i < group->childCount(); ++i) {
            QTreeWidgetItem *item = group->child(i);
            if (item->checkState(0) == Qt::Checked)
                result << item->data(0, Qt::UserRole).toString();
        }
    }
    return result;
}

QStringList KMimeTypeChooser::patterns() const
{
    QStringList result;
    foreach (const QString &name, mimeTypes()) {
        KMimeType::Ptr mt = KMimeType::mimeType(name);
        if (mt)
            result += mt->patterns();
    }
    return result;
}

KMimeTypeChooserDialog::KMimeTypeChooserDialog(const QString &caption, const QString &text, const QStringList &selectedMimeTypes,
                                               const QString &defaultGroup, const QStringList &groupsToShow, int visuals, QWidget *parent)
    : KDialog(parent), m_chooser(0)
{
    init(caption, text, selectedMimeTypes, defaultGroup, groupsToShow, visuals);
}

KMimeTypeChooserDialog::KMimeTypeChooserDialog(const QString &caption, const QString &text, const QStringList &selectedMimeTypes,
                                               const QString &defaultGroup, QWidget *parent)
    : KDialog(parent), m_chooser(0)
{
    init(caption, text, selectedMimeTypes, defaultGroup, QStringList(),
         KMimeTypeChooser::Comments | KMimeTypeChooser::Patterns | KMimeTypeChooser::EditButton);
}

void KMimeTypeChooserDialog::init(const QString &caption, const QString &text, const QStringList &selectedMimeTypes,
                                  const QString &defaultGroup, const QStringList &groupsToShow, int visuals)
{
    setCaption(caption.isEmpty() ? i18n("Select Mime Types") : caption);
    setButtons(Cancel | Ok);
    setModal(true);
    setDefaultButton(Ok);
    m_chooser = new KMimeTypeChooser(text, selectedMimeTypes, defaultGroup, groupsToShow, visuals, this);
    setMainWidget(m_chooser);
    KConfigGroup group(KGlobal::config(), "KMimeTypeChooserDialog");
    resize(group.readEntry("size", QSize(500, 400)));
}

KMimeTypeChooserDialog::~KMimeTypeChooserDialog()
{
    KConfigGroup group(KGlobal::config(), "KMimeTypeChooserDialog");
    group.writeEntry("size", size());
}

KMimeTypeChooser *KMimeTypeChooserDialog::chooser()
{
    return m_chooser;
}
