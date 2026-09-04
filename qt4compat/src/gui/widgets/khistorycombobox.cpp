/* qt4compat: khistorycombobox.cpp - see khistorycombobox.h */
#include "khistorycombobox.h"

#include <kglobalsettings.h>

KHistoryComboBox::KHistoryComboBox(QWidget *parent)
    : KComboBox(parent)
{
    init(true);
}

KHistoryComboBox::KHistoryComboBox(bool useCompletion, QWidget *parent)
    : KComboBox(parent)
{
    init(useCompletion);
}

KHistoryComboBox::~KHistoryComboBox()
{
}

void KHistoryComboBox::init(bool useCompletion)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setDuplicatesEnabled(false);
    if (!useCompletion) {
        setCompletionMode(KGlobalSettings::CompletionNone);
    }
}

void KHistoryComboBox::setHistoryItems(QStringList items, bool setCompletionListToo)
{
    blockSignals(true);
    clear();

    QStringList cleaned;
    for (int i = 0; i < items.size(); ++i) {
        if (!items.at(i).isEmpty()) {
            cleaned << items.at(i);
        }
    }
    if (maxCount() > 0 && cleaned.size() > maxCount()) {
        cleaned = cleaned.mid(0, maxCount());
    }
    insertItems(0, cleaned);
    blockSignals(false);

    if (setCompletionListToo && compObj()) {
        compObj()->setItems(cleaned);
    }
}

QStringList KHistoryComboBox::historyItems() const
{
    QStringList items;
    for (int i = 0; i < count(); ++i) {
        items << itemText(i);
    }
    return items;
}

bool KHistoryComboBox::removeFromHistory(const QString &item)
{
    if (item.isEmpty()) {
        return false;
    }
    const int idx = findText(item);
    if (idx == -1) {
        return false;
    }
    removeItem(idx);
    return true;
}

void KHistoryComboBox::addToHistory(const QString &item)
{
    if (item.isEmpty()) {
        return;
    }

    // an existing duplicate is dropped so the new entry moves to the top
    const int idx = findText(item);
    if (idx != -1) {
        removeItem(idx);
    }

    insertItem(0, item);
    if (maxCount() > 0) {
        while (count() > maxCount()) {
            removeItem(count() - 1);
        }
    }
    setCurrentIndex(0);
}

void KHistoryComboBox::clearHistory()
{
    const QString text = currentText();
    clear();
    if (isEditable()) {
        setEditText(text);
    }
    emit cleared();
}
