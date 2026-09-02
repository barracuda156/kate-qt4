/* qt4compat: kservicetypetrader.cpp */
#include <kservicetypetrader.h>

KServiceTypeTrader::KServiceTypeTrader()
{
}

KServiceTypeTrader *KServiceTypeTrader::self()
{
    static KServiceTypeTrader instance;
    return &instance;
}

KService::List KServiceTypeTrader::query(const QString &, const QString &) const
{
    return KService::List();
}

KService::Ptr KServiceTypeTrader::preferredService(const QString &) const
{
    return KService::Ptr();
}
