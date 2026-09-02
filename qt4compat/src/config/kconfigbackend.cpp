/*
   This file is part of the KDE libraries
   Copyright (c) 2006 Thomas Braxton <brax108@cox.net>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997-1999 Matthias Kalle Dalheimer <kalle@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
/* qt4compat: vendored from kdelibs 4.14 kdecore/config/kconfigbackend.cpp, changes: no backend plugins (INI only), moc via qt4_wrap_cpp */


#include "kconfigbackend.h"

#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "kglobal.h"
#include "kconfig.h"
#include "kconfigini_p.h"
#include "kconfigdata.h"
#include "kdebug.h"
#include "kstandarddirs.h"

typedef KSharedPtr<KConfigBackend> BackendPtr;

class KConfigBackend::Private
{
public:
    qint64 size;
    QDateTime lastModified;
    QString localFileName;

    static QString whatSystem(const QString& /*fileName*/)
    {
        return QLatin1String("INI");
    }
};


void KConfigBackend::registerMappings(const KEntryMap& /*entryMap*/)
{
}

BackendPtr KConfigBackend::create(const KComponentData& componentData, const QString& file,
                                  const QString& sys)
{
    Q_UNUSED(componentData);
    //qDebug() << "creating a backend for file" << file << "with system" << sys;
    const QString system = (sys.isEmpty() ? Private::whatSystem(file) : sys);
    KConfigBackend* backend = 0;

    if (system.compare(QLatin1String("INI"), Qt::CaseInsensitive) != 0) {
        // qt4compat: no backend plugins, everything is INI
        kWarning() << "unknown config backend" << system << "- using INI";
    }

    //qDebug() << "default creation of the Ini backend";
    backend = new KConfigIniBackend;
    backend->setFilePath(file);
    return BackendPtr(backend);
}

KConfigBackend::KConfigBackend()
 : d(new Private)
{
}

KConfigBackend::~KConfigBackend()
{
    delete d;
}

QDateTime KConfigBackend::lastModified() const
{
    return d->lastModified;
}

void KConfigBackend::setLastModified(const QDateTime& dt)
{
    d->lastModified = dt;
}

qint64 KConfigBackend::size() const
{
    return d->size;
}

void KConfigBackend::setSize(qint64 sz)
{
    d->size = sz;
}

QString KConfigBackend::filePath() const
{
    return d->localFileName;
}

void KConfigBackend::setLocalFilePath(const QString& file)
{
    d->localFileName = file;
}
