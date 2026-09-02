/* qt4compat: kuser.h */
#ifndef KDECOMPAT_KUSER_H
#define KDECOMPAT_KUSER_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <kdemacros.h>

#include <sys/types.h>

class KDECORE_EXPORT KUser
{
public:
    enum UIDMode { UseEffectiveUID, UseRealUserID };
    enum UserProperty { FullName, RoomNumber, WorkPhone, HomePhone };

    explicit KUser(UIDMode mode = UseEffectiveUID);
    explicit KUser(uid_t uid);
    explicit KUser(const QString &name);
    explicit KUser(const char *name);
    KUser(const KUser &user);
    KUser &operator=(const KUser &user);
    bool operator==(const KUser &user) const;
    bool operator!=(const KUser &user) const { return !operator==(user); }
    ~KUser();

    bool isValid() const;
    uid_t uid() const;
    gid_t gid() const;
    bool isSuperUser() const;
    QString loginName() const;
    QString homeDir() const;
    QString faceIconPath() const;
    QString shell() const;
    QStringList groupNames() const;
    QVariant property(UserProperty which) const;
    static QList<KUser> allUsers();
    static QStringList allUserNames();

private:
    void fill(uid_t uid, const QString &name);
    bool m_valid;
    uid_t m_uid;
    gid_t m_gid;
    QString m_login;
    QString m_home;
    QString m_shell;
    QString m_fullName;
};

#endif
