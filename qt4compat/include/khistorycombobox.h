/* qt4compat: khistorycombobox.h - trimmed local replacement for kdelibs'
 * KHistoryComboBox. Upstream keeps a separate history list plus pixmap
 * providers, KConfig persistence helpers and up/down rotation; Kate's search
 * plugin (addons/kate/search) only needs an editable combo box that remembers
 * what was typed into it (newest entry first, no duplicates) and can save/
 * restore that list as a QStringList, so here the combo box's own item list
 * *is* the history - no separate storage. Public API names match kdelibs so
 * callers written against it compile unmodified. */
#ifndef KDECOMPAT_KHISTORYCOMBOBOX_H
#define KDECOMPAT_KHISTORYCOMBOBOX_H

#include <kdeui_export.h>
#include <kcombobox.h>

class KDEUI_EXPORT KHistoryComboBox : public KComboBox
{
    Q_OBJECT
public:
    explicit KHistoryComboBox(QWidget *parent = 0);
    explicit KHistoryComboBox(bool useCompletion, QWidget *parent = 0);
    virtual ~KHistoryComboBox();

    void setHistoryItems(QStringList items, bool setCompletionListToo = false);
    QStringList historyItems() const;
    bool removeFromHistory(const QString &item);

public Q_SLOTS:
    void addToHistory(const QString &item);
    void clearHistory();

Q_SIGNALS:
    void cleared();

private:
    void init(bool useCompletion);
};

#endif
