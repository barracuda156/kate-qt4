/* qt4compat: ktip.h - tip of the day is not shipped; all no-ops */
#ifndef KDECOMPAT_KTIP_H
#define KDECOMPAT_KTIP_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>

class QWidget;

class KDEUI_EXPORT KTipDatabase
{
public:
    explicit KTipDatabase(const QString &tipFile = QString());
    explicit KTipDatabase(const QStringList &tipFiles);
    ~KTipDatabase();
    QString tip() const;
    void nextTip();
    void prevTip();
};

class KDEUI_EXPORT KTipDialog
{
public:
    static void showTip(QWidget *parent, const QString &tipFile = QString(), bool force = false);
    static void showMultiTip(QWidget *parent, const QStringList &tipFiles, bool force = false);
    static void showTip(const QString &tipFile = QString(), bool force = false);
    static void setShowOnStart(bool show);
};

#endif
