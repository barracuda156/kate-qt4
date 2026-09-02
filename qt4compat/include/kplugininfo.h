/* qt4compat: kplugininfo.h - plugin metadata holder (value class) */
#ifndef KDECOMPAT_KPLUGININFO_H
#define KDECOMPAT_KPLUGININFO_H

#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <kdemacros.h>
#include <kconfiggroup.h>
#include <kservice.h>

class KPluginInfoPrivate;

class KDECORE_EXPORT KPluginInfo
{
public:
    typedef QList<KPluginInfo> List;

    explicit KPluginInfo(const QString &filename, const char *resource = 0);
    explicit KPluginInfo(const KService::Ptr service);
    KPluginInfo();
    KPluginInfo(const KPluginInfo &rhs);
    KPluginInfo &operator=(const KPluginInfo &rhs);
    bool operator==(const KPluginInfo &rhs) const;
    bool operator!=(const KPluginInfo &rhs) const { return !operator==(rhs); }
    bool operator<(const KPluginInfo &rhs) const;
    bool operator>(const KPluginInfo &rhs) const;
    ~KPluginInfo();

    static KPluginInfo::List fromServices(const KService::List &services, const KConfigGroup &config = KConfigGroup());
    static KPluginInfo::List fromFiles(const QStringList &files, const KConfigGroup &config = KConfigGroup());
    static KPluginInfo::List fromKPartsInstanceName(const QString &componentName, const KConfigGroup &config = KConfigGroup());

    bool isHidden() const;
    void setPluginEnabled(bool enabled);
    bool isPluginEnabled() const;
    bool isPluginEnabledByDefault() const;
    QVariant property(const QString &key) const;
    QString name() const;
    QString comment() const;
    QString icon() const;
    QString entryPath() const;
    QString author() const;
    QString email() const;
    QString category() const;
    QString pluginName() const;
    QString version() const;
    QString website() const;
    QString license() const;
    QStringList dependencies() const;
    KService::Ptr service() const;
    QList<KService::Ptr> kcmServices() const;
    void setConfig(const KConfigGroup &config);
    KConfigGroup config() const;
    void save(KConfigGroup config = KConfigGroup());
    void load(const KConfigGroup &config = KConfigGroup());
    void defaults();
    bool isValid() const;

    /* qt4compat: fill in metadata for statically linked plugins */
    void setName(const QString &name);
    void setPluginName(const QString &pluginName);
    void setComment(const QString &comment);
    void setIcon(const QString &icon);
    void setEnabledByDefault(bool on);
    void setDependencies(const QStringList &deps);

private:
    QSharedDataPointer<KPluginInfoPrivate> d;
};

#endif
