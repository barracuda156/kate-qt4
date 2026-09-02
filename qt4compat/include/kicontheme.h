/* qt4compat: kicontheme.h - KIconTheme stub; the bundled theme is handled by QIcon::fromTheme */
#ifndef KDECOMPAT_KICONTHEME_H
#define KDECOMPAT_KICONTHEME_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <kiconloader.h>

class QAction;

class KDEUI_EXPORT KIconTheme
{
public:
    explicit KIconTheme(const QString &name, const QString &appName = QString());
    ~KIconTheme();

    QString name() const;
    QString internalName() const;
    QString description() const;
    QString example() const;
    QString screenshot() const;
    QStringList inherits() const;
    bool isValid() const;
    bool isHidden() const;
    int depth() const;
    QString dir() const;
    int defaultSize(KIconLoader::Group group) const;
    QList<int> querySizes(KIconLoader::Group group) const;
    QStringList queryIcons(int size, KIconLoader::Context context = KIconLoader::Any) const;
    QStringList queryIconsByContext(int size, KIconLoader::Context context = KIconLoader::Any) const;
    bool hasContext(KIconLoader::Context context) const;
    QString iconPath(const QString &name, int size, KIconLoader::MatchType match) const;

    static QStringList list();
    static QString current();
    static void reconfigure();
    static QString defaultThemeName();
    static void assignIconsToContextMenu(int type, QList<QAction *> actions);

private:
    QString m_name;
};

#endif
