/* qt4compat: kservice.h - KService without ksycoca. There are no services:
 * every lookup returns a null pointer and every list is empty. */
#ifndef KDECOMPAT_KSERVICE_H
#define KDECOMPAT_KSERVICE_H

#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <kdemacros.h>
#include <ksharedptr.h>

class KServicePrivate;

class KDECORE_EXPORT KService : public QSharedData
{
public:
    typedef KSharedPtr<KService> Ptr;
    typedef QList<Ptr> List;

    enum DBusStartupType { DBusNone = 0, DBusUnique, DBusMulti, DBusWait };

    KService(const QString &name, const QString &exec, const QString &icon);
    explicit KService(const QString &fullpath);
    virtual ~KService();

    bool isValid() const;
    bool isApplication() const;
    QString name() const;
    QString exec() const;
    QString library() const;
    QString icon() const;
    bool terminal() const;
    QString terminalOptions() const;
    QString desktopEntryName() const;
    QString desktopEntryPath() const;
    QString entryPath() const;
    QString menuId() const;
    QString storageId() const;
    QString path() const;
    QString comment() const;
    QString genericName() const;
    QString untranslatedGenericName() const;
    QStringList keywords() const;
    QStringList categories() const;
    QStringList mimeTypes() const;
    QStringList serviceTypes() const;
    bool hasServiceType(const QString &serviceTypePtr) const;
    bool hasMimeType(const QString &mimeType) const;
    bool allowAsDefault() const;
    bool allowMultipleFiles() const;
    int initialPreference() const;
    bool noDisplay() const;
    bool showInKDE() const;
    QString parentApp() const;
    QString pluginKeyword() const;
    QString docPath() const;
    QVariant property(const QString &name, QVariant::Type t = QVariant::Invalid) const;
    QStringList propertyNames() const;
    DBusStartupType dbusStartupType() const;
    QString locateLocal() const;

    template <class T>
    T *createInstance(QObject *parent = 0, const QVariantList &args = QVariantList(), QString *error = 0) const
    {
        Q_UNUSED(parent)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("plugin loading is not available");
        return 0;
    }
    template <class T>
    T *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args = QVariantList(), QString *error = 0) const
    {
        Q_UNUSED(parentWidget)
        Q_UNUSED(parent)
        Q_UNUSED(args)
        if (error)
            *error = QLatin1String("plugin loading is not available");
        return 0;
    }

    static Ptr serviceByName(const QString &name);
    static Ptr serviceByDesktopPath(const QString &path);
    static Ptr serviceByDesktopName(const QString &name);
    static Ptr serviceByMenuId(const QString &menuId);
    static Ptr serviceByStorageId(const QString &storageId);
    static List allServices();

private:
    KServicePrivate *const d;
};

#endif
