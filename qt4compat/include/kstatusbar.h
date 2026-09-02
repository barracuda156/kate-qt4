/* qt4compat: kstatusbar.h - vendored from kdelibs 4.14 kdeui/widgets/kstatusbar.h
 * (LGPL 2, (C) 1997,1998, 2000 Sven Radej), unchanged API */
#ifndef KDECOMPAT_KSTATUSBAR_H
#define KDECOMPAT_KSTATUSBAR_H

#include <QtGui/QStatusBar>
#include <kdemacros.h>

class QObject;
class QEvent;
class KStatusBarPrivate;

class KDEUI_EXPORT KStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit KStatusBar(QWidget *parent = 0);
    ~KStatusBar();

    void insertItem(const QString &text, int id, int stretch = 0);
    void insertPermanentItem(const QString &text, int id, int stretch = 0);
    void insertFixedItem(const QString &text, int id);
    void insertPermanentFixedItem(const QString &text, int id);
    void removeItem(int id);
    bool hasItem(int id) const;
    QString itemText(int id) const;
    void changeItem(const QString &text, int id);
    void setItemAlignment(int id, Qt::Alignment alignment);
    void setItemFixed(int id, int width = -1);

Q_SIGNALS:
    void pressed(int);
    void released(int);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    KStatusBarPrivate *const d;
};

#endif
