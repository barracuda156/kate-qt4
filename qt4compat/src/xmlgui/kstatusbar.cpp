/* qt4compat: vendored from kdelibs 4.14 kdeui/widgets/kstatusbar.cpp (LGPL 2,
 * (C) 1997,1998, 2000 Sven Radej), changes: size grip default from platform, no .moc include */
#include <kstatusbar.h>

#include <QtCore/QEvent>
#include <QtCore/QHash>
#include <QtGui/QLabel>

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <ksharedconfig.h>
#include <ksqueezedtextlabel.h>

class KStatusBarPrivate
{
public:
    int id(QObject *object) const
    {
        QHash<int, QLabel *>::const_iterator it = qFind(items, object);
        if (it != items.constEnd())
            return it.key();
        return -1;
    }

    QHash<int, QLabel *> items;
};

bool KStatusBar::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        const int id = d->id(object);
        if (id > -1) {
            emit pressed(id);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        const int id = d->id(object);
        if (id > -1) {
            emit released(id);
            return true;
        }
    }
    return QStatusBar::eventFilter(object, event);
}

KStatusBar::KStatusBar(QWidget *parent)
    : QStatusBar(parent), d(new KStatusBarPrivate)
{
    KConfigGroup group(KGlobal::config(), QLatin1String("StatusBar style"));
#ifdef Q_OS_MAC
    const bool grip_enabled = group.readEntry(QLatin1String("SizeGripEnabled"), true);
#else
    const bool grip_enabled = group.readEntry(QLatin1String("SizeGripEnabled"), false);
#endif
    setSizeGripEnabled(grip_enabled);
}

KStatusBar::~KStatusBar()
{
    delete d;
}

void KStatusBar::insertItem(const QString &text, int id, int stretch)
{
    if (d->items[id])
        kDebug() << "KStatusBar::insertItem: item id " << id << " already exists.";

    KSqueezedTextLabel *l = new KSqueezedTextLabel(text, this);
    l->installEventFilter(this);
    l->setFixedHeight(fontMetrics().height() + 2);
    l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->items.insert(id, l);
    addPermanentWidget(l, stretch);
    l->show();
}

void KStatusBar::insertFixedItem(const QString &text, int id)
{
    insertItem(text, id, 0);
    setItemFixed(id);
}

void KStatusBar::insertPermanentItem(const QString &text, int id, int stretch)
{
    if (d->items[id])
        kDebug() << "KStatusBar::insertPermanentItem: item id " << id << " already exists.";

    QLabel *l = new QLabel(text, this);
    l->installEventFilter(this);
    l->setFixedHeight(fontMetrics().height() + 2);
    l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->items.insert(id, l);
    addPermanentWidget(l, stretch);
    l->show();
}

void KStatusBar::insertPermanentFixedItem(const QString &text, int id)
{
    insertPermanentItem(text, id, 0);
    setItemFixed(id);
}

void KStatusBar::removeItem(int id)
{
    if (d->items.contains(id)) {
        QLabel *label = d->items[id];
        removeWidget(label);
        d->items.remove(id);
        delete label;
    } else {
        kDebug() << "KStatusBar::removeItem: bad item id: " << id;
    }
}

bool KStatusBar::hasItem(int id) const
{
    return d->items.contains(id);
}

QString KStatusBar::itemText(int id) const
{
    if (!hasItem(id))
        return QString();
    return d->items[id]->text();
}

void KStatusBar::changeItem(const QString &text, int id)
{
    QLabel *label = d->items[id];
    KSqueezedTextLabel *squeezed = qobject_cast<KSqueezedTextLabel *>(label);

    if (squeezed) {
        squeezed->setText(text);
    } else if (label) {
        label->setText(text);
        if (label->minimumWidth() != label->maximumWidth())
            reformat();
    } else {
        kDebug() << "KStatusBar::changeItem: bad item id: " << id;
    }
}

void KStatusBar::setItemAlignment(int id, Qt::Alignment alignment)
{
    QLabel *label = qobject_cast<QLabel *>(d->items[id]);
    if (label)
        label->setAlignment(alignment);
    else
        kDebug() << "KStatusBar::setItemAlignment: bad item id: " << id;
}

void KStatusBar::setItemFixed(int id, int w)
{
    QLabel *label = qobject_cast<QLabel *>(d->items[id]);
    if (label) {
        if (w == -1)
            w = fontMetrics().boundingRect(label->text()).width() + 3;
        label->setFixedWidth(w);
    } else {
        kDebug() << "KStatusBar::setItemFixed: bad item id: " << id;
    }
}
