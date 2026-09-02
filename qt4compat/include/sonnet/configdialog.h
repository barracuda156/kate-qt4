/* qt4compat: sonnet/configdialog.h */
#ifndef KDECOMPAT_SONNET_CONFIGDIALOG_H
#define KDECOMPAT_SONNET_CONFIGDIALOG_H

#include <QtGui/QDialog>
#include <kdemacros.h>

class KConfig;

namespace Sonnet
{
    class ConfigWidget;

    class KDEUI_EXPORT ConfigDialog : public QDialog
    {
        Q_OBJECT
    public:
        ConfigDialog(KConfig *config, QWidget *parent);
        ~ConfigDialog();

        void setLanguage(const QString &language);
        QString language() const;

    protected Q_SLOTS:
        virtual void slotOk();
        virtual void slotApply();

    Q_SIGNALS:
        void languageChanged(const QString &language);
        void configChanged();

    private:
        ConfigWidget *m_widget;
        Q_DISABLE_COPY(ConfigDialog)
    };
}

#endif
