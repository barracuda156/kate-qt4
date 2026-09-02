/* qt4compat: kicontheme.cpp */
#include <kicontheme.h>

#include <QtGui/QIcon>

KIconTheme::KIconTheme(const QString &name, const QString &)
    : m_name(name)
{
}

KIconTheme::~KIconTheme()
{
}

QString KIconTheme::name() const { return m_name; }
QString KIconTheme::internalName() const { return m_name; }
QString KIconTheme::description() const { return QString(); }
QString KIconTheme::example() const { return QLatin1String("folder"); }
QString KIconTheme::screenshot() const { return QString(); }
QStringList KIconTheme::inherits() const { return QStringList(); }
bool KIconTheme::isValid() const { return !m_name.isEmpty(); }
bool KIconTheme::isHidden() const { return false; }
int KIconTheme::depth() const { return 32; }
QString KIconTheme::dir() const { return QLatin1String(":/icons/") + m_name; }

int KIconTheme::defaultSize(KIconLoader::Group group) const
{
    return KIconLoader::global()->currentSize(group);
}

QList<int> KIconTheme::querySizes(KIconLoader::Group) const
{
    QList<int> sizes;
    sizes << 16 << 22 << 32 << 48;
    return sizes;
}

QStringList KIconTheme::queryIcons(int, KIconLoader::Context) const { return QStringList(); }
QStringList KIconTheme::queryIconsByContext(int, KIconLoader::Context) const { return QStringList(); }
bool KIconTheme::hasContext(KIconLoader::Context) const { return true; }

QString KIconTheme::iconPath(const QString &name, int size, KIconLoader::MatchType) const
{
    return KIconLoader::global()->iconPath(name, size, true);
}

QStringList KIconTheme::list()
{
    return QStringList() << current();
}

QString KIconTheme::current()
{
    const QString name = QIcon::themeName();
    return name.isEmpty() ? defaultThemeName() : name;
}

void KIconTheme::reconfigure()
{
}

QString KIconTheme::defaultThemeName()
{
    return QLatin1String("oxygen");
}

void KIconTheme::assignIconsToContextMenu(int, QList<QAction *>)
{
}
