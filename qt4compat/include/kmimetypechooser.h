/* qt4compat: kmimetypechooser.h - tree of mime types with check boxes */
#ifndef KDECOMPAT_KMIMETYPECHOOSER_H
#define KDECOMPAT_KMIMETYPECHOOSER_H

#include <QtCore/QStringList>
#include <kdemacros.h>
#include <kdialog.h>
#include <kvbox.h>

class QTreeWidget;
class QTreeWidgetItem;

class KDEUI_EXPORT KMimeTypeChooser : public KVBox
{
    Q_OBJECT
public:
    enum Visuals {
        Comments = 1,
        Patterns = 2,
        EditButton = 4
    };

    explicit KMimeTypeChooser(const QString &text = QString(), const QStringList &selectedMimeTypes = QStringList(),
                              const QString &defaultGroup = QString(), const QStringList &groupsToShow = QStringList(),
                              int visuals = Comments | Patterns | EditButton, QWidget *parent = 0);
    ~KMimeTypeChooser();

    QStringList mimeTypes() const;
    QStringList patterns() const;

private:
    void loadMimeTypes(const QStringList &selected, const QString &defaultGroup, const QStringList &groupsToShow, int visuals);
    QTreeWidget *m_tree;
    QStringList m_mimeTypes;
};

class KDEUI_EXPORT KMimeTypeChooserDialog : public KDialog
{
public:
    explicit KMimeTypeChooserDialog(const QString &caption = QString(), const QString &text = QString(),
                                    const QStringList &selectedMimeTypes = QStringList(),
                                    const QString &defaultGroup = QString(), const QStringList &groupsToShow = QStringList(),
                                    int visuals = KMimeTypeChooser::Comments | KMimeTypeChooser::Patterns | KMimeTypeChooser::EditButton,
                                    QWidget *parent = 0);
    KMimeTypeChooserDialog(const QString &caption, const QString &text, const QStringList &selectedMimeTypes,
                           const QString &defaultGroup, QWidget *parent = 0);
    ~KMimeTypeChooserDialog();

    KMimeTypeChooser *chooser();

private:
    void init(const QString &caption, const QString &text, const QStringList &selectedMimeTypes,
              const QString &defaultGroup, const QStringList &groupsToShow, int visuals);
    KMimeTypeChooser *m_chooser;
};

#endif
