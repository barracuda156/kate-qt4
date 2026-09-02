/* qt4compat: kglobal.cpp */
#include <kglobal.h>
#include "kglobal_p.h"

#include <kcharsets.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <ksharedconfig.h>
#include <kstandarddirs.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QSet>

#include <sys/stat.h>
#include <sys/types.h>

namespace {

struct KGlobalData
{
    KGlobalData() : mainComponent(0), activeComponent(0), fallbackDirs(0), locale(0), refCount(0), allowQuit(false) {}
    ~KGlobalData()
    {
        delete activeComponent;
        delete mainComponent;
        delete fallbackDirs;
        delete locale;
    }
    KComponentData *mainComponent;
    KComponentData *activeComponent;
    KStandardDirs *fallbackDirs;
    KLocale *locale;
    int refCount;
    bool allowQuit;
};

static KGlobalData *globalData()
{
    static KGlobalData data;
    return &data;
}

} // namespace

void KGlobal::setMainComponent(const KComponentData &d)
{
    KGlobalData *g = globalData();
    if (g->mainComponent)
        *g->mainComponent = d;
    else
        g->mainComponent = new KComponentData(d);
}

const KComponentData &KGlobal::mainComponent()
{
    KGlobalData *g = globalData();
    if (!g->mainComponent) {
        /* nobody registered a component yet: synthesize one from the
         * application name so that config and resource lookup keep working */
        QByteArray name = QCoreApplication::instance() ? QCoreApplication::applicationName().toUtf8() : QByteArray();
        if (name.isEmpty())
            name = "kate";
        g->mainComponent = new KComponentData(name, name, KComponentData::SkipMainComponentRegistration);
    }
    return *g->mainComponent;
}

bool KGlobal::hasMainComponent()
{
    return globalData()->mainComponent != 0;
}

KComponentData KGlobal::activeComponent()
{
    KGlobalData *g = globalData();
    if (g->activeComponent)
        return *g->activeComponent;
    return mainComponent();
}

void KGlobal::setActiveComponent(const KComponentData &d)
{
    KGlobalData *g = globalData();
    delete g->activeComponent;
    g->activeComponent = d.isValid() ? new KComponentData(d) : 0;
}

KSharedConfigPtr KGlobal::config()
{
    return mainComponent().config();
}

KStandardDirs *KGlobal::dirs()
{
    KGlobalData *g = globalData();
    if (g->mainComponent)
        return g->mainComponent->dirs();
    if (!g->fallbackDirs)
        g->fallbackDirs = new KStandardDirs;
    return g->fallbackDirs;
}

KLocale *KGlobal::locale()
{
    KGlobalData *g = globalData();
    if (!g->locale)
        g->locale = new KLocale;
    return g->locale;
}

bool KGlobal::hasLocale()
{
    return globalData()->locale != 0;
}

void KGlobal::setLocale(KLocale *locale, CopyCatalogs)
{
    KGlobalData *g = globalData();
    if (g->locale != locale) {
        delete g->locale;
        g->locale = locale;
    }
}

KCharsets *KGlobal::charsets()
{
    return KCharsets::charsets();
}

QString KGlobal::caption()
{
    if (QCoreApplication::instance())
        return QCoreApplication::applicationName();
    return QString();
}

const QString &KGlobal::staticQString(const char *str)
{
    return staticQString(QString::fromLatin1(str));
}

const QString &KGlobal::staticQString(const QString &str)
{
    static QSet<QString> *strings = new QSet<QString>;
    QSet<QString>::const_iterator it = strings->constFind(str);
    if (it == strings->constEnd())
        it = strings->insert(str);
    return *it;
}

void KGlobal::ref()
{
    ++globalData()->refCount;
}

void KGlobal::deref()
{
    KGlobalData *g = globalData();
    if (--g->refCount <= 0 && g->allowQuit && QCoreApplication::instance())
        QCoreApplication::instance()->quit();
}

void KGlobal::setAllowQuit(bool allowQuit)
{
    globalData()->allowQuit = allowQuit;
}

mode_t KGlobal::umask()
{
    static mode_t mask = 0;
    static bool done = false;
    if (!done) {
        mask = ::umask(0);
        ::umask(mask);
        done = true;
    }
    return mask;
}
