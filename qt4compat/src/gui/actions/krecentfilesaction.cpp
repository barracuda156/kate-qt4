/* qt4compat: krecentfilesaction.cpp */
#include <krecentfilesaction.h>
#include "kselectaction_p.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandarddirs.h>

#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtGui/QActionGroup>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QFontMetrics>

#include <climits>

class KRecentFilesActionPrivate : public KSelectActionPrivate
{
public:
    KRecentFilesActionPrivate() : m_maxItems(10), m_noEntriesAction(0), clearSeparator(0), clearAction(0) {}
    int m_maxItems;
    QMap<QAction *, QString> m_shortNames;
    QMap<QAction *, KUrl> m_urls;
    QAction *m_noEntriesAction;
    QAction *clearSeparator;
    QAction *clearAction;
};

KRecentFilesAction::KRecentFilesAction(QObject *parent)
    : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
    init();
}

KRecentFilesAction::KRecentFilesAction(const QString &text, QObject *parent)
    : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
    init();
    setText(text);
}

KRecentFilesAction::KRecentFilesAction(const KIcon &icon, const QString &text, QObject *parent)
    : KSelectAction(*new KRecentFilesActionPrivate, parent)
{
    init();
    setIcon(icon);
    setText(text);
}

void KRecentFilesAction::init()
{
    Q_D(KRecentFilesAction);
    setToolBarMode(KSelectAction::MenuMode);
    setMenuAccelsEnabled(false);

    d->m_noEntriesAction = new QAction(i18n("No Entries"), selectableActionGroup());
    d->m_noEntriesAction->setEnabled(false);
    d->m_noEntriesAction->setActionGroup(0);
    menu()->addAction(d->m_noEntriesAction);

    d->clearSeparator = menu()->addSeparator();
    d->clearSeparator->setVisible(false);
    d->clearAction = menu()->addAction(i18n("Clear List"), this, SLOT(clear()));
    d->clearAction->setVisible(false);
    setEnabled(false);

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(urlActionTriggered(QAction*)));
}

KRecentFilesAction::~KRecentFilesAction()
{
}

void KRecentFilesAction::urlActionTriggered(QAction *action)
{
    Q_D(KRecentFilesAction);
    if (d->m_urls.contains(action))
        emit urlSelected(d->m_urls.value(action));
}

static QString titleWithSensibleWidth(const QString &nameValue, const QString &value)
{
    int maxWidthForTitles = INT_MAX;
    if (QApplication::instance()) {
        const QDesktopWidget *desktop = QApplication::desktop();
        for (int i = 0; i < desktop->screenCount(); ++i)
            maxWidthForTitles = qMin(maxWidthForTitles, desktop->availableGeometry(i).width() * 3 / 4);
    }
    const QFontMetrics fontMetrics = QFontMetrics(QFont());
    QString title = nameValue + QLatin1String(" [") + value + QLatin1Char(']');
    if (fontMetrics.width(title) > maxWidthForTitles) {
        const int nameValueMaxWidth = maxWidthForTitles * 3 / 4;
        const int nameWidth = fontMetrics.width(nameValue);
        QString cutNameValue, cutValue;
        if (nameWidth > nameValueMaxWidth) {
            cutNameValue = fontMetrics.elidedText(nameValue, Qt::ElideMiddle, nameValueMaxWidth);
            cutValue = fontMetrics.elidedText(value, Qt::ElideMiddle, maxWidthForTitles - nameValueMaxWidth);
        } else {
            cutNameValue = nameValue;
            cutValue = fontMetrics.elidedText(value, Qt::ElideMiddle, maxWidthForTitles - nameWidth);
        }
        title = cutNameValue + QLatin1String(" [") + cutValue + QLatin1Char(']');
    }
    return title;
}

void KRecentFilesAction::addUrl(const KUrl &_url, const QString &name)
{
    Q_D(KRecentFilesAction);
    const KUrl url(_url);

    if (url.isLocalFile() && !KGlobal::dirs()->relativeLocation("tmp", url.toLocalFile()).startsWith(QLatin1Char('/')))
        return;
    const QString tmpName = name.isEmpty() ? url.fileName() : name;
    const QString pathOrUrl(url.pathOrUrl());

    /* remove an existing entry for the same url */
    for (QMap<QAction *, KUrl>::ConstIterator it = d->m_urls.constBegin(); it != d->m_urls.constEnd(); ++it) {
        if (it.value() == url) {
            QAction *a = it.key();
            removeAction(a)->deleteLater();
            break;
        }
    }
    /* remove the oldest entry if the list is full */
    if (d->m_maxItems && selectableActionGroup()->actions().count() == d->m_maxItems) {
        QAction *action = selectableActionGroup()->actions().first();
        removeAction(action)->deleteLater();
    }

    d->m_noEntriesAction->setVisible(false);
    d->clearSeparator->setVisible(true);
    d->clearAction->setVisible(true);
    setEnabled(true);

    QString title = titleWithSensibleWidth(tmpName, pathOrUrl);
    title.replace(QLatin1Char('&'), QLatin1String("&&"));
    addAction(new KAction(title, selectableActionGroup()), url, tmpName);
}

void KRecentFilesAction::addAction(QAction *action, const KUrl &url, const QString &name)
{
    Q_D(KRecentFilesAction);
    menu()->insertAction(menu()->actions().value(0), action);
    d->m_shortNames.insert(action, name);
    d->m_urls.insert(action, url);
    action->setActionGroup(selectableActionGroup());
    action->setCheckable(true);
}

QAction *KRecentFilesAction::removeAction(QAction *action)
{
    Q_D(KRecentFilesAction);
    KSelectAction::removeAction(action);
    d->m_shortNames.remove(action);
    d->m_urls.remove(action);
    if (selectableActionGroup()->actions().isEmpty()) {
        d->m_noEntriesAction->setVisible(true);
        d->clearSeparator->setVisible(false);
        d->clearAction->setVisible(false);
        setEnabled(false);
    }
    return action;
}

void KRecentFilesAction::removeUrl(const KUrl &url)
{
    Q_D(KRecentFilesAction);
    for (QMap<QAction *, KUrl>::ConstIterator it = d->m_urls.constBegin(); it != d->m_urls.constEnd(); ++it) {
        if (it.value() == url) {
            delete removeAction(it.key());
            return;
        }
    }
}

KUrl::List KRecentFilesAction::urls() const
{
    Q_D(const KRecentFilesAction);
    return KUrl::List(d->m_urls.values());
}

void KRecentFilesAction::clear()
{
    clearEntries();
    emit recentListCleared();
}

void KRecentFilesAction::clearEntries()
{
    Q_D(KRecentFilesAction);
    KSelectAction::clear();
    d->m_shortNames.clear();
    d->m_urls.clear();
    d->m_noEntriesAction->setVisible(true);
    d->clearSeparator->setVisible(false);
    d->clearAction->setVisible(false);
    setEnabled(false);
}

void KRecentFilesAction::loadEntries(const KConfigGroup &_config)
{
    Q_D(KRecentFilesAction);
    clearEntries();

    KConfigGroup cg = _config;
    if (cg.name().isEmpty())
        cg = KConfigGroup(cg.config(), QLatin1String("RecentFiles"));

    bool thereAreEntriesToShow = false;
    for (int i = 1; i <= d->m_maxItems; i++) {
        const QString key = QString::fromLatin1("File%1").arg(i);
        const QString value = cg.readPathEntry(key, QString());
        if (value.isEmpty())
            continue;
        const KUrl url(value);
        if (url.isLocalFile() && !QFile::exists(url.toLocalFile()))
            continue;
        if (d->m_urls.values().contains(url))
            continue;

        const QString nameKey = QString::fromLatin1("Name%1").arg(i);
        const QString nameValue = cg.readPathEntry(nameKey, url.fileName());
        QString title = titleWithSensibleWidth(nameValue, url.pathOrUrl());
        title.replace(QLatin1Char('&'), QLatin1String("&&"));
        thereAreEntriesToShow = true;
        addAction(new KAction(title, selectableActionGroup()), url, nameValue);
    }
    d->m_noEntriesAction->setVisible(!thereAreEntriesToShow);
    d->clearSeparator->setVisible(thereAreEntriesToShow);
    d->clearAction->setVisible(thereAreEntriesToShow);
    setEnabled(thereAreEntriesToShow);
}

void KRecentFilesAction::saveEntries(const KConfigGroup &_cg)
{
    Q_D(KRecentFilesAction);
    KConfigGroup cg = _cg;
    if (cg.name().isEmpty())
        cg = KConfigGroup(cg.config(), QLatin1String("RecentFiles"));
    cg.deleteGroup();

    const QList<QAction *> lst = selectableActionGroup()->actions();
    for (int i = 1; i <= lst.count(); i++) {
        const QString key = QString::fromLatin1("File%1").arg(i);
        cg.writePathEntry(key, d->m_urls.value(lst.at(i - 1)).pathOrUrl());
        const QString nameKey = QString::fromLatin1("Name%1").arg(i);
        cg.writePathEntry(nameKey, d->m_shortNames.value(lst.at(i - 1)));
    }
}

int KRecentFilesAction::maxItems() const
{
    Q_D(const KRecentFilesAction);
    return d->m_maxItems;
}

void KRecentFilesAction::setMaxItems(int maxItems)
{
    Q_D(KRecentFilesAction);
    d->m_maxItems = maxItems;
    while (selectableActionGroup()->actions().count() > maxItems)
        delete removeAction(selectableActionGroup()->actions().first());
}
