/* qt4compat: kmenubar.h - plain QMenuBar (native on macOS); no X11 top-level menu bar */
#ifndef KDECOMPAT_KMENUBAR_H
#define KDECOMPAT_KMENUBAR_H

#include <QtGui/QMenuBar>
#include <kdemacros.h>

class KDEUI_EXPORT KMenuBar : public QMenuBar
{
    Q_OBJECT
    Q_PROPERTY(bool topLevelMenu READ isTopLevelMenu WRITE setTopLevelMenu)
public:
    explicit KMenuBar(QWidget *parent = 0);
    ~KMenuBar();

    void setTopLevelMenu(bool top_level = true);
    bool isTopLevelMenu() const;
};

#endif
