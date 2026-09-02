/* qt4compat: kparts/browserextension.h - minimal KParts::BrowserExtension (signals and action state only) */
#ifndef KDECOMPAT_KPARTS_BROWSEREXTENSION_H
#define KDECOMPAT_KPARTS_BROWSEREXTENSION_H

#include <QtCore/QDataStream>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kparts/event.h>
#include <kparts/part.h>
#include <kurl.h>

#include <sys/types.h>

class QAction;

namespace KParts
{

class ReadOnlyPart;

class KPARTS_EXPORT BrowserArguments
{
public:
    BrowserArguments();
    BrowserArguments(const BrowserArguments &args);
    BrowserArguments &operator=(const BrowserArguments &args);
    virtual ~BrowserArguments();

    QStringList docState;
    bool softReload;
    QByteArray postData;
    void setContentType(const QString &contentType);
    QString contentType() const;
    void setDoPost(bool enable);
    bool doPost() const;
    void setRedirectedRequest(bool redirected);
    bool redirectedRequest() const;
    void setLockHistory(bool lock);
    bool lockHistory() const;
    void setNewTab(bool newTab);
    bool newTab() const;
    QString frameName;
    bool trustedSource;
    bool forcesNewWindow() const;
    void setForcesNewWindow(bool forcesNewWindow);

private:
    QString m_contentType;
    bool m_doPost;
    bool m_redirectedRequest;
    bool m_lockHistory;
    bool m_newTab;
    bool m_forcesNewWindow;
};

class KPARTS_EXPORT OpenUrlEvent : public Event
{
public:
    OpenUrlEvent(ReadOnlyPart *part, const KUrl &url, const OpenUrlArguments &args = OpenUrlArguments(),
                 const BrowserArguments &browserArgs = BrowserArguments());
    virtual ~OpenUrlEvent();

    ReadOnlyPart *part() const;
    KUrl url() const;
    OpenUrlArguments arguments() const;
    BrowserArguments browserArguments() const;
    static bool test(const QEvent *event);

private:
    ReadOnlyPart *m_part;
    KUrl m_url;
    OpenUrlArguments m_args;
    BrowserArguments m_browserArgs;
};

typedef QMap<QString, QList<QAction *> > ActionGroupMap;

class KPARTS_EXPORT BrowserExtension : public QObject
{
    Q_OBJECT
public:
    explicit BrowserExtension(ReadOnlyPart *parent);
    virtual ~BrowserExtension();

    typedef uint PopupFlags;
    enum PopupFlag {
        DefaultPopupItems = 0x0000,
        ShowNavigationItems = 0x0001,
        ShowUp = 0x0002,
        ShowReload = 0x0004,
        ShowBookmark = 0x0008,
        ShowCreateNewWindow = 0x0010,
        ShowTextSelectionItems = 0x0020,
        NoDeletion = 0x0040,
        IsLink = 0x0080,
        ShowUrlOperations = 0x0100,
        ShowProperties = 0x200
    };

    virtual void setBrowserArguments(const BrowserArguments &args);
    BrowserArguments browserArguments() const;
    int xOffset();
    int yOffset();
    virtual void saveState(QDataStream &stream);
    virtual void restoreState(QDataStream &stream);
    bool isURLDropHandlingEnabled() const;
    void setURLDropHandlingEnabled(bool enable);
    bool isActionEnabled(const char *name) const;
    QString actionText(const char *name) const;
    static BrowserExtension *childObject(QObject *obj);

Q_SIGNALS:
#if !defined(Q_MOC_RUN)
public:
#endif
    void enableAction(const char *name, bool enabled);
    void setActionText(const char *name, const QString &text);
    void openUrlRequest(const KUrl &url, const KParts::OpenUrlArguments &arguments = KParts::OpenUrlArguments(),
                        const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments());
    void openUrlRequestDelayed(const KUrl &url, const KParts::OpenUrlArguments &arguments,
                               const KParts::BrowserArguments &browserArguments);
    void openUrlNotify();
    void setLocationBarUrl(const QString &url);
    void setIconUrl(const KUrl &url);
    void createNewWindow(const KUrl &url, const KParts::OpenUrlArguments &arguments = KParts::OpenUrlArguments(),
                         const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments());
    void loadingProgress(int percent);
    void speedProgress(int bytesPerSecond);
    void infoMessage(const QString &);
    void popupMenu(const QPoint &global, const KUrl &url);
    void popupMenu(const QPoint &global, const KUrl &url, mode_t mode,
                   const KParts::OpenUrlArguments &arguments = KParts::OpenUrlArguments(),
                   const KParts::BrowserArguments &browserArguments = KParts::BrowserArguments(),
                   KParts::BrowserExtension::PopupFlags flags = KParts::BrowserExtension::DefaultPopupItems,
                   const KParts::ActionGroupMap &actionGroups = KParts::ActionGroupMap());
    void selectionInfo(const KUrl::List &urls);
    void selectionInfo(const QString &text);
    void moveTopLevelWidget(int x, int y);
    void resizeTopLevelWidget(int w, int h);
    void requestFocus(KParts::ReadOnlyPart *part);

public Q_SLOTS:
    void pasteRequest();

private Q_SLOTS:
    void slotEnableAction(const char *name, bool enabled);
    void slotSetActionText(const char *name, const QString &text);
    void slotCompleted();
    void slotOpenUrlRequest(const KUrl &url, const KParts::OpenUrlArguments &arguments,
                            const KParts::BrowserArguments &browserArguments);

private:
    class BrowserExtensionPrivate;
    BrowserExtensionPrivate *const d;
};

}

#endif
