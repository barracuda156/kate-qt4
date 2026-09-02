/* qt4compat: kauthorized.h - everything is authorized (no KIOSK). */
#ifndef KDECOMPAT_KAUTHORIZED_H
#define KDECOMPAT_KAUTHORIZED_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>

class KUrl;

namespace KAuthorized
{
    inline bool authorize(const QString &) { return true; }
    inline bool authorizeKAction(const QString &) { return true; }
    inline bool authorizeUrlAction(const QString &, const KUrl &, const KUrl &) { return true; }
    inline bool authorizeControlModule(const QString &) { return true; }
    inline QStringList authorizeControlModules(const QStringList &modules) { return modules; }
}

#endif
