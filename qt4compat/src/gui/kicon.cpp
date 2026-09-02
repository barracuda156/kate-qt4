/* qt4compat: kicon.cpp */
#include <kicon.h>
#include <kiconloader.h>

#include <QtCore/QDir>
#include <QtCore/QFile>

KIcon::KIcon(const QString &iconName, KIconLoader *, const QStringList &)
    : QIcon()
{
    KIconLoader::global();
    if (iconName.isEmpty())
        return;
    if (QDir::isAbsolutePath(iconName) && QFile::exists(iconName)) {
        addFile(iconName);
        return;
    }
    QIcon::operator=(QIcon::fromTheme(iconName));
}

KIcon::KIcon(const QIcon &copy)
    : QIcon(copy)
{
}

KIcon::KIcon()
    : QIcon()
{
}

KIcon::~KIcon()
{
}
