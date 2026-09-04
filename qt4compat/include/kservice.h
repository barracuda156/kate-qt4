/* qt4compat: kservice.h - KService without ksycoca. A service is a parsed
 * .desktop file. There is no service database: KServiceTypeTrader and the
 * static lookups below collect every services/<name>.desktop found in the
 * KStandardDirs "data" directories (the "services" resource type), so a
 * plugin's .desktop file is installed with
 * kate_install_data(services FILES <plugin>.desktop). A service that names
 * a plugin library (X-KDE-Library) is only usable when that plugin is linked
 * into the application, see kpluginloader.h (K_IMPORT_PLUGIN). Property
 * values are plain strings, as they are in kdelibs for service types that
 * declare no property types. */
#ifndef KDECOMPAT_KSERVICE_H
#define KDECOMPAT_KSERVICE_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <kdemacros.h>
#include <klocale.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <ksharedptr.h>

class KServicePrivate;
class QWidget;

class KDECORE_EXPORT KService : public QSharedData
{
public:
    typedef KSharedPtr<KService> Ptr;
    typedef QList<Ptr> List;

    enum DBusStartupType { DBusNone = 0, DBusUnique, DBusMulti, DBusWait };

    KService(const QString &name, const QString &exec, const QString &icon);
    /* parses the .desktop file at fullpath ([Desktop Entry] group) */
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
        return createInstance<T>(parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent) : 0, parent, args, error);
    }
    template <class T>
    T *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args = QVariantList(), QString *error = 0) const
    {
        QObject *o = createInstanceInternal(T::staticMetaObject.className(), parentWidget, parent, args, error);
        T *t = qobject_cast<T *>(o);
        if (!t)
            delete o;
        return t;
    }

    static Ptr serviceByName(const QString &name);
    static Ptr serviceByDesktopPath(const QString &path);
    static Ptr serviceByDesktopName(const QString &name);
    static Ptr serviceByMenuId(const QString &menuId);
    static Ptr serviceByStorageId(const QString &storageId);
    /* every services/*.desktop in the data directories, parsed once */
    static List allServices();

private:
    QObject *createInstanceInternal(const char *iface, QWidget *parentWidget, QObject *parent,
                                    const QVariantList &args, QString *error) const;
    KServicePrivate *const d;
};

#endif
