/* qt4compat: kservicetypetrader.cpp - services come from the .desktop files
 * in the "services" resource directories (see kservice.h). Constraints are
 * not evaluated (Kate never passes one). A service naming a plugin library
 * is listed only when that plugin is linked into the application, so a
 * .desktop file left behind for a plugin that is not built cannot produce a
 * plugin that fails to load. */
#include <kservicetypetrader.h>

#include <kdebug.h>
#include <kpluginloader.h>

KServiceTypeTrader::KServiceTypeTrader()
{
}

KServiceTypeTrader *KServiceTypeTrader::self()
{
    static KServiceTypeTrader instance;
    return &instance;
}

KService::List KServiceTypeTrader::query(const QString &servicetype, const QString &constraint) const
{
    if (!constraint.isEmpty())
        kDebug() << "ignoring trader constraint" << constraint << "for" << servicetype;
    KService::List result;
    foreach (const KService::Ptr &s, KService::allServices()) {
        if (!s->hasServiceType(servicetype))
            continue;
        if (!s->library().isEmpty() && !KPluginLoader::hasStaticPlugin(s->library())) {
            kDebug() << "service" << s->entryPath() << "needs plugin" << s->library()
                     << "which is not built into this application";
            continue;
        }
        result << s;
    }
    return result;
}

KService::Ptr KServiceTypeTrader::preferredService(const QString &serviceType) const
{
    const KService::List all = query(serviceType);
    return all.isEmpty() ? KService::Ptr() : all.first();
}
