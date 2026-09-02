/* qt4compat: kcolordialog.cpp */
#include <kcolordialog.h>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDialog>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

int KColorDialog::getColor(QColor &theColor, QWidget *parent)
{
    const QColor initial = theColor.isValid() ? theColor : QColor(Qt::white);
    const QColor c = QColorDialog::getColor(initial, parent);
    if (!c.isValid())
        return QDialog::Rejected;
    theColor = c;
    return QDialog::Accepted;
}

int KColorDialog::getColor(QColor &theColor, const QColor &defaultColor, QWidget *parent)
{
    const QColor initial = theColor.isValid() ? theColor : defaultColor;
    const QColor c = QColorDialog::getColor(initial.isValid() ? initial : QColor(Qt::white), parent);
    if (!c.isValid())
        return QDialog::Rejected;
    theColor = c;
    return QDialog::Accepted;
}

QColor KColorDialog::grabColor(const QPoint &p)
{
    QWidget *desktop = QApplication::desktop();
    QPixmap pm = QPixmap::grabWindow(desktop->winId(), p.x(), p.y(), 1, 1);
    QImage i = pm.toImage();
    return i.pixel(0, 0);
}
