/* qt4compat: kmimetypetrader.cpp */
#include <kmimetypetrader.h>

KMimeTypeTrader::KMimeTypeTrader()
{
}

KMimeTypeTrader *KMimeTypeTrader::self()
{
    static KMimeTypeTrader instance;
    return &instance;
}

KService::List KMimeTypeTrader::query(const QString &, const QString &, const QString &) const
{
    return KService::List();
}

KService::Ptr KMimeTypeTrader::preferredService(const QString &, const QString &)
{
    return KService::Ptr();
}
