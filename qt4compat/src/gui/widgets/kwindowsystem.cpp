/* qt4compat: kwindowsystem.cpp */
#include <kwindowsystem.h>

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QDesktopWidget>

static QWidget *widgetFor(WId win)
{
    QWidget *w = QWidget::find(win);
    return w ? w->window() : 0;
}

QString KWindowInfo::visibleName() const
{
    QWidget *w = widgetFor(m_win);
    return w ? w->windowTitle() : QString();
}

QString KWindowInfo::name() const
{
    return visibleName();
}

bool KWindowInfo::isMinimized() const
{
    QWidget *w = widgetFor(m_win);
    return w ? w->isMinimized() : false;
}

QRect KWindowInfo::geometry() const
{
    QWidget *w = widgetFor(m_win);
    return w ? w->geometry() : QRect();
}

QRect KWindowInfo::frameGeometry() const
{
    QWidget *w = widgetFor(m_win);
    return w ? w->frameGeometry() : QRect();
}

KWindowSystem *KWindowSystem::self()
{
    static KWindowSystem *s = 0;
    if (!s)
        s = new KWindowSystem;
    return s;
}

QList<WId> KWindowSystem::windows()
{
    QList<WId> list;
    foreach (QWidget *w, QApplication::topLevelWidgets())
        list << w->winId();
    return list;
}

KWindowInfo KWindowSystem::windowInfo(WId win, unsigned long, unsigned long)
{
    return KWindowInfo(win);
}

bool KWindowSystem::hasWId(WId id)
{
    return QWidget::find(id) != 0;
}

QList<WId> KWindowSystem::stackingOrder()
{
    return windows();
}

WId KWindowSystem::activeWindow()
{
    QWidget *w = QApplication::activeWindow();
    return w ? w->winId() : 0;
}

void KWindowSystem::activateWindow(WId win, long)
{
    if (QWidget *w = widgetFor(win)) {
        if (w->isMinimized())
            w->showNormal();
        w->raise();
        w->activateWindow();
    }
}

void KWindowSystem::forceActiveWindow(WId win, long time)
{
    activateWindow(win, time);
}

void KWindowSystem::demandAttention(WId win, bool set)
{
    if (set)
        if (QWidget *w = widgetFor(win))
            QApplication::alert(w);
}

void KWindowSystem::raiseWindow(WId win)
{
    if (QWidget *w = widgetFor(win))
        w->raise();
}

void KWindowSystem::lowerWindow(WId win)
{
    if (QWidget *w = widgetFor(win))
        w->lower();
}

bool KWindowSystem::compositingActive()
{
    return true;
}

int KWindowSystem::currentDesktop() { return 1; }
int KWindowSystem::numberOfDesktops() { return 1; }
void KWindowSystem::setCurrentDesktop(int) {}
void KWindowSystem::setOnAllDesktops(WId, bool) {}
void KWindowSystem::setOnDesktop(WId, int) {}

void KWindowSystem::setMainWindow(QWidget *subwindow, WId mainwindow)
{
    Q_UNUSED(subwindow)
    Q_UNUSED(mainwindow)
}

WId KWindowSystem::transientFor(WId) { return 0; }
WId KWindowSystem::groupLeader(WId) { return 0; }

QPixmap KWindowSystem::icon(WId win, int width, int height, bool)
{
    QWidget *w = widgetFor(win);
    if (!w)
        return QPixmap();
    return w->windowIcon().pixmap(width > 0 ? width : 32, height > 0 ? height : 32);
}

void KWindowSystem::setIcons(WId win, const QPixmap &icon, const QPixmap &)
{
    if (QWidget *w = widgetFor(win))
        w->setWindowIcon(QIcon(icon));
}

void KWindowSystem::setType(WId, int) {}
void KWindowSystem::setState(WId, unsigned long) {}
void KWindowSystem::clearState(WId, unsigned long) {}

void KWindowSystem::minimizeWindow(WId win, bool)
{
    if (QWidget *w = widgetFor(win))
        w->showMinimized();
}

void KWindowSystem::unminimizeWindow(WId win, bool)
{
    if (QWidget *w = widgetFor(win))
        w->showNormal();
}

QRect KWindowSystem::workArea(int)
{
    return QApplication::desktop()->availableGeometry();
}

QString KWindowSystem::desktopName(int) { return QString(); }
void KWindowSystem::setDesktopName(int, const QString &) {}
bool KWindowSystem::showingDesktop() { return false; }
void KWindowSystem::allowExternalProcessWindowActivation(int) {}
void KWindowSystem::allowMoveResize(WId) {}
bool KWindowSystem::icccmCompliantMappingState() { return true; }
