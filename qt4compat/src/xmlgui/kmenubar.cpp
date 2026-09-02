/* qt4compat: kmenubar.cpp */
#include <kmenubar.h>

KMenuBar::KMenuBar(QWidget *parent)
    : QMenuBar(parent)
{
}

KMenuBar::~KMenuBar()
{
}

void KMenuBar::setTopLevelMenu(bool)
{
}

bool KMenuBar::isTopLevelMenu() const
{
    return false;
}
