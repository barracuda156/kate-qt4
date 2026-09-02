/* qt4compat: browserextension.cpp */
#include <kparts/browserextension.h>

#include <QtCore/QMap>

using namespace KParts;

BrowserArguments::BrowserArguments()
    : softReload(false), trustedSource(false), m_doPost(false), m_redirectedRequest(false),
      m_lockHistory(false), m_newTab(false), m_forcesNewWindow(false)
{
}

BrowserArguments::BrowserArguments(const BrowserArguments &args)
{
    *this = args;
}

BrowserArguments &BrowserArguments::operator=(const BrowserArguments &args)
{
    if (this == &args)
        return *this;
    docState = args.docState;
    softReload = args.softReload;
    postData = args.postData;
    frameName = args.frameName;
    trustedSource = args.trustedSource;
    m_contentType = args.m_contentType;
    m_doPost = args.m_doPost;
    m_redirectedRequest = args.m_redirectedRequest;
    m_lockHistory = args.m_lockHistory;
    m_newTab = args.m_newTab;
    m_forcesNewWindow = args.m_forcesNewWindow;
    return *this;
}

BrowserArguments::~BrowserArguments()
{
}

void BrowserArguments::setContentType(const QString &contentType) { m_contentType = contentType; }
QString BrowserArguments::contentType() const { return m_contentType; }
void BrowserArguments::setDoPost(bool enable) { m_doPost = enable; }
bool BrowserArguments::doPost() const { return m_doPost; }
void BrowserArguments::setRedirectedRequest(bool redirected) { m_redirectedRequest = redirected; }
bool BrowserArguments::redirectedRequest() const { return m_redirectedRequest; }
void BrowserArguments::setLockHistory(bool lock) { m_lockHistory = lock; }
bool BrowserArguments::lockHistory() const { return m_lockHistory; }
void BrowserArguments::setNewTab(bool newTab) { m_newTab = newTab; }
bool BrowserArguments::newTab() const { return m_newTab; }
bool BrowserArguments::forcesNewWindow() const { return m_forcesNewWindow; }
void BrowserArguments::setForcesNewWindow(bool forcesNewWindow) { m_forcesNewWindow = forcesNewWindow; }

OpenUrlEvent::OpenUrlEvent(ReadOnlyPart *part, const KUrl &url, const OpenUrlArguments &args, const BrowserArguments &browserArgs)
    : Event("KParts/BrowserExtension/OpenURLevent"), m_part(part), m_url(url), m_args(args), m_browserArgs(browserArgs)
{
}

OpenUrlEvent::~OpenUrlEvent()
{
}

ReadOnlyPart *OpenUrlEvent::part() const { return m_part; }
KUrl OpenUrlEvent::url() const { return m_url; }
OpenUrlArguments OpenUrlEvent::arguments() const { return m_args; }
BrowserArguments OpenUrlEvent::browserArguments() const { return m_browserArgs; }

bool OpenUrlEvent::test(const QEvent *event)
{
    return Event::test(event, "KParts/BrowserExtension/OpenURLevent");
}

class BrowserExtension::BrowserExtensionPrivate
{
public:
    BrowserExtensionPrivate(ReadOnlyPart *parent) : m_part(parent), m_urlDropHandlingEnabled(false) {}
    ReadOnlyPart *m_part;
    BrowserArguments m_browserArguments;
    bool m_urlDropHandlingEnabled;
    QMap<QByteArray, bool> m_actionStatus;
    QMap<QByteArray, QString> m_actionText;
};

BrowserExtension::BrowserExtension(ReadOnlyPart *parent)
    : QObject(parent), d(new BrowserExtensionPrivate(parent))
{
    connect(this, SIGNAL(enableAction(const char*,bool)), this, SLOT(slotEnableAction(const char*,bool)));
    connect(this, SIGNAL(setActionText(const char*,QString)), this, SLOT(slotSetActionText(const char*,QString)));
    connect(this, SIGNAL(openUrlRequest(KUrl,KParts::OpenUrlArguments,KParts::BrowserArguments)),
            this, SLOT(slotOpenUrlRequest(KUrl,KParts::OpenUrlArguments,KParts::BrowserArguments)));
    if (d->m_part)
        connect(d->m_part, SIGNAL(completed()), this, SLOT(slotCompleted()));
}

BrowserExtension::~BrowserExtension()
{
    delete d;
}

void BrowserExtension::setBrowserArguments(const BrowserArguments &args)
{
    d->m_browserArguments = args;
}

BrowserArguments BrowserExtension::browserArguments() const
{
    return d->m_browserArguments;
}

int BrowserExtension::xOffset()
{
    return 0;
}

int BrowserExtension::yOffset()
{
    return 0;
}

void BrowserExtension::saveState(QDataStream &stream)
{
    stream << d->m_part->url() << (qint32)xOffset() << (qint32)yOffset();
}

void BrowserExtension::restoreState(QDataStream &stream)
{
    KUrl u;
    qint32 xOfs, yOfs;
    stream >> u >> xOfs >> yOfs;
    OpenUrlArguments args;
    args.setXOffset(xOfs);
    args.setYOffset(yOfs);
    d->m_part->setArguments(args);
    d->m_part->openUrl(u);
}

bool BrowserExtension::isURLDropHandlingEnabled() const
{
    return d->m_urlDropHandlingEnabled;
}

void BrowserExtension::setURLDropHandlingEnabled(bool enable)
{
    d->m_urlDropHandlingEnabled = enable;
}

bool BrowserExtension::isActionEnabled(const char *name) const
{
    return d->m_actionStatus.value(QByteArray(name), false);
}

QString BrowserExtension::actionText(const char *name) const
{
    return d->m_actionText.value(QByteArray(name));
}

BrowserExtension *BrowserExtension::childObject(QObject *obj)
{
    foreach (QObject *child, obj->children()) {
        if (KParts::BrowserExtension *ext = qobject_cast<KParts::BrowserExtension *>(child))
            return ext;
    }
    return 0;
}

void BrowserExtension::pasteRequest()
{
}

void BrowserExtension::slotEnableAction(const char *name, bool enabled)
{
    d->m_actionStatus.insert(QByteArray(name), enabled);
}

void BrowserExtension::slotSetActionText(const char *name, const QString &text)
{
    d->m_actionText.insert(QByteArray(name), text);
}

void BrowserExtension::slotCompleted()
{
}

void BrowserExtension::slotOpenUrlRequest(const KUrl &url, const KParts::OpenUrlArguments &arguments, const KParts::BrowserArguments &browserArguments)
{
    emit openUrlRequestDelayed(url, arguments, browserArguments);
}
