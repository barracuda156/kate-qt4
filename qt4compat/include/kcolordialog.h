/* qt4compat: kcolordialog.h - QColorDialog based */
#ifndef KDECOMPAT_KCOLORDIALOG_H
#define KDECOMPAT_KCOLORDIALOG_H

#include <QtCore/QPoint>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <kdemacros.h>

class KDEUI_EXPORT KColorDialog
{
public:
    static int getColor(QColor &theColor, QWidget *parent = 0);
    static int getColor(QColor &theColor, const QColor &defaultColor, QWidget *parent = 0);
    static QColor grabColor(const QPoint &p);
};

#endif
