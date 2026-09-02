/* qt4compat: knotification.cpp */
#include <knotification.h>

#include <QtGui/QApplication>

KNotification *KNotification::event(const QString &, const QString &, const QPixmap &, QWidget *, const NotificationFlags &, const QString &)
{
    return 0;
}

KNotification *KNotification::event(StandardEvent, const QString &, const QPixmap &, QWidget *, const NotificationFlags &)
{
    return 0;
}

void KNotification::beep(const QString &, QWidget *)
{
    QApplication::beep();
}
