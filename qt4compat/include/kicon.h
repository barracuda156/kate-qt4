/* qt4compat: kicon.h */
#ifndef KDECOMPAT_KICON_H
#define KDECOMPAT_KICON_H

#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <kdemacros.h>

class KIconLoader;

class KDEUI_EXPORT KIcon : public QIcon
{
public:
    explicit KIcon(const QString &iconName, KIconLoader *iconLoader = 0, const QStringList &overlays = QStringList());
    explicit KIcon(const QIcon &copy);
    KIcon();
    ~KIcon();
};

#endif
