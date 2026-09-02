/* qt4compat: sonnet/configwidget.h */
#ifndef KDECOMPAT_SONNET_CONFIGWIDGET_H
#define KDECOMPAT_SONNET_CONFIGWIDGET_H

#include <QtGui/QWidget>
#include <kdemacros.h>

class KConfig;

namespace Sonnet
{
    class KDEUI_EXPORT ConfigWidget : public QWidget
    {
        Q_OBJECT
    public:
        ConfigWidget(KConfig *config, QWidget *parent);
        ~ConfigWidget();

        bool backgroundCheckingButtonShown() const;
        void setLanguage(const QString &language);
        QString language() const;

    public Q_SLOTS:
        void save();
        void setBackgroundCheckingButtonShown(bool shown);
        void slotDefault();

    protected Q_SLOTS:
        void slotChanged();

    Q_SIGNALS:
        void configChanged();

    private:
        class Private;
        Private *const d;
    };
}

#endif
