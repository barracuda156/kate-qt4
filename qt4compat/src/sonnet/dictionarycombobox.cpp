/* qt4compat: sonnet/dictionarycombobox.cpp */
#include <sonnet/dictionarycombobox.h>

#include <klocalizedstring.h>

namespace Sonnet {

DictionaryComboBox::DictionaryComboBox(QWidget *parent)
    : QComboBox(parent)
{
    reloadCombo();
    connect(this, SIGNAL(activated(int)), this, SLOT(slotDictionaryChanged(int)));
}

DictionaryComboBox::~DictionaryComboBox()
{
}

void DictionaryComboBox::reloadCombo()
{
    clear();
    addItem(i18n("No spell checking available"), QString());
    setCurrentIndex(0);
    setEnabled(false);
}

QString DictionaryComboBox::currentDictionaryName() const
{
    return currentText();
}

QString DictionaryComboBox::currentDictionary() const
{
    return itemData(currentIndex()).toString();
}

void DictionaryComboBox::setCurrentByDictionaryName(const QString &)
{
}

void DictionaryComboBox::setCurrentByDictionary(const QString &)
{
}

void DictionaryComboBox::slotDictionaryChanged(int index)
{
    emit dictionaryChanged(itemData(index).toString());
    emit dictionaryNameChanged(itemText(index));
}

} // namespace Sonnet
