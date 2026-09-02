/* qt4compat: krecentfilesaction.h (kdelibs 4.14 API) */
#ifndef KDECOMPAT_KRECENTFILESACTION_H
#define KDECOMPAT_KRECENTFILESACTION_H

#include <kselectaction.h>
#include <kurl.h>

class KConfigGroup;
class KRecentFilesActionPrivate;

class KDEUI_EXPORT KRecentFilesAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY(int maxItems READ maxItems WRITE setMaxItems)
    Q_DECLARE_PRIVATE(KRecentFilesAction)
public:
    explicit KRecentFilesAction(QObject *parent);
    KRecentFilesAction(const QString &text, QObject *parent);
    KRecentFilesAction(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KRecentFilesAction();

    void addAction(QAction *action, const KUrl &url, const QString &name);
    virtual QAction *removeAction(QAction *action);

public Q_SLOTS:
    virtual void clear();

public:
    int maxItems() const;
    void setMaxItems(int maxItems);
    void loadEntries(const KConfigGroup &config);
    void saveEntries(const KConfigGroup &config);
    void addUrl(const KUrl &url, const QString &name = QString());
    void removeUrl(const KUrl &url);
    KUrl::List urls() const;

Q_SIGNALS:
    void urlSelected(const KUrl &url);
    void recentListCleared();

private Q_SLOTS:
    void urlActionTriggered(QAction *action);

private:
    void init();
    void clearEntries();
    using KSelectAction::addAction;
};

#endif
