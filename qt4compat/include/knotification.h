/* qt4compat: knotification.h - only beep() does anything */
#ifndef KDECOMPAT_KNOTIFICATION_H
#define KDECOMPAT_KNOTIFICATION_H

#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <kdemacros.h>

class QWidget;

class KDEUI_EXPORT KNotification
{
public:
    enum StandardEvent { Notification, Warning, Error, Catastrophe };
    enum NotificationFlag {
        RaiseWidgetOnActivation = 0x01,
        CloseOnTimeout = 0x00,
        Persistent = 0x02,
        CloseWhenWidgetActivated = 0x04,
        DefaultEvent = 0xF000
    };
    Q_DECLARE_FLAGS(NotificationFlags, NotificationFlag)

    static KNotification *event(const QString &eventId, const QString &text = QString(), const QPixmap &pixmap = QPixmap(),
                                QWidget *widget = 0, const NotificationFlags &flags = CloseOnTimeout,
                                const QString &componentName = QString());
    static KNotification *event(StandardEvent eventId, const QString &text = QString(), const QPixmap &pixmap = QPixmap(),
                                QWidget *widget = 0, const NotificationFlags &flags = CloseOnTimeout);
    static void beep(const QString &reason = QString(), QWidget *widget = 0);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KNotification::NotificationFlags)

#endif
