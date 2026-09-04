/* qt4compat: kapplication.cpp */
#include <kapplication.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <QtCore/QEvent>
#include <QtGui/QFileOpenEvent>

static KApplication *s_kapp = 0;

/* The layer's Qt resources (data/xmlgui.qrc: ui_standards.rc; data/icons.qrc:
 * the icon theme) are compiled into the static library. A .qrc registers
 * itself through a static initializer in its own object file, which the
 * linker only pulls in when something references it - nothing did, so
 * neither resource existed at run time: the standard menu layout was not
 * merged (Kate's Settings menu had one entry) and icon lookups failed.
 * Register them explicitly from here, code every application runs. */
static void initQt4CompatResources()
{
    static bool done = false;
    if (done)
        return;
    done = true;
    Q_INIT_RESOURCE(xmlgui);
    Q_INIT_RESOURCE(icons);
}

KApplication::KApplication(bool GUIenabled)
    : QApplication(KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv(), GUIenabled)
{
    initQt4CompatResources();
    s_kapp = this;
    const KAboutData *about = KCmdLineArgs::aboutData();
    if (about) {
        setApplicationName(about->appName());
        setApplicationVersion(about->version());
        setOrganizationDomain(about->organizationDomain());
        if (!KGlobal::hasMainComponent()) {
            KComponentData data(about);
            Q_UNUSED(data)
        }
    } else if (!KGlobal::hasMainComponent()) {
        KComponentData data(applicationName().toUtf8());
        Q_UNUSED(data)
    }
    /* make sure the bundled icon theme is registered before any KIcon is created */
    KIconLoader::global();
    connect(this, SIGNAL(aboutToQuit()), this, SIGNAL(saveYourself()));
}

KApplication::~KApplication()
{
    s_kapp = 0;
}

KApplication *KApplication::kApplication()
{
    return s_kapp;
}

KConfig *KApplication::sessionConfig()
{
    return 0;
}

void KApplication::disableSessionManagement()
{
}

void KApplication::enableSessionManagement()
{
}

void KApplication::setTopWidget(QWidget *)
{
}

bool KApplication::sessionSaving()
{
    return false;
}

QString KApplication::tempSaveName(const QString &pFilename)
{
    return pFilename;
}

QString KApplication::checkRecoverFile(const QString &pFilename, bool &bRecover)
{
    bRecover = false;
    return pFilename;
}

void KApplication::updateUserTimestamp(int)
{
}

QByteArray KApplication::startupId() const
{
    return QByteArray();
}

void KApplication::setStartupId(const QByteArray &)
{
}

void KApplication::clearStartupId()
{
}

unsigned long KApplication::userTimestamp() const
{
    return 0;
}

void KApplication::updateRemoteUserTimestamp(const QString &, int)
{
}

void KApplication::setQuitOnLastWindowClosed(bool quit)
{
    QApplication::setQuitOnLastWindowClosed(quit);
}

bool KApplication::event(QEvent *e)
{
    if (e->type() == QEvent::FileOpen) {
        QFileOpenEvent *fo = static_cast<QFileOpenEvent *>(e);
        QUrl url = fo->url();
        if (url.isEmpty() && !fo->file().isEmpty())
            url = QUrl::fromLocalFile(fo->file());
        if (!url.isEmpty()) {
            emit openURLRequest(url);
            return true;
        }
    }
    return QApplication::event(e);
}

void KApplication::quit()
{
    QApplication::quit();
}
