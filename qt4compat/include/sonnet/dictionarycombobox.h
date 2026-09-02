/* qt4compat: sonnet/dictionarycombobox.h - one entry, no dictionaries */
#ifndef KDECOMPAT_SONNET_DICTIONARYCOMBOBOX_H
#define KDECOMPAT_SONNET_DICTIONARYCOMBOBOX_H

#include <QtGui/QComboBox>
#include <kdemacros.h>

namespace Sonnet
{
    class KDEUI_EXPORT DictionaryComboBox : public QComboBox
    {
        Q_OBJECT
    public:
        explicit DictionaryComboBox(QWidget *parent = 0);
        ~DictionaryComboBox();

        void reloadCombo();
        QString currentDictionaryName() const;
        QString currentDictionary() const;
        void setCurrentByDictionaryName(const QString &dictionaryName);
        void setCurrentByDictionary(const QString &dictionary);

    Q_SIGNALS:
        void dictionaryChanged(const QString &dictionary);
        void dictionaryNameChanged(const QString &dictionaryName);

    private Q_SLOTS:
        void slotDictionaryChanged(int index);
    };
}

#endif
