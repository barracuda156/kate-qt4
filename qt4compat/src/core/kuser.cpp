/* qt4compat: kuser.cpp */
#include <kuser.h>

#include <QtCore/QFile>

#include <pwd.h>
#include <unistd.h>

KUser::KUser(UIDMode mode)
    : m_valid(false), m_uid(0), m_gid(0)
{
    fill(mode == UseEffectiveUID ? ::geteuid() : ::getuid(), QString());
}

KUser::KUser(uid_t uid)
    : m_valid(false), m_uid(0), m_gid(0)
{
    fill(uid, QString());
}

KUser::KUser(const QString &name)
    : m_valid(false), m_uid(0), m_gid(0)
{
    fill(0, name);
}

KUser::KUser(const char *name)
    : m_valid(false), m_uid(0), m_gid(0)
{
    fill(0, QString::fromLocal8Bit(name));
}

KUser::KUser(const KUser &user)
    : m_valid(user.m_valid), m_uid(user.m_uid), m_gid(user.m_gid), m_login(user.m_login),
      m_home(user.m_home), m_shell(user.m_shell), m_fullName(user.m_fullName)
{
}

KUser &KUser::operator=(const KUser &user)
{
    m_valid = user.m_valid;
    m_uid = user.m_uid;
    m_gid = user.m_gid;
    m_login = user.m_login;
    m_home = user.m_home;
    m_shell = user.m_shell;
    m_fullName = user.m_fullName;
    return *this;
}

bool KUser::operator==(const KUser &user) const
{
    return m_valid && user.m_valid && m_uid == user.m_uid;
}

KUser::~KUser()
{
}

void KUser::fill(uid_t uid, const QString &name)
{
    struct passwd *p = name.isEmpty() ? ::getpwuid(uid) : ::getpwnam(QFile::encodeName(name).constData());
    if (!p)
        return;
    m_valid = true;
    m_uid = p->pw_uid;
    m_gid = p->pw_gid;
    m_login = QString::fromLocal8Bit(p->pw_name);
    m_home = QFile::decodeName(p->pw_dir);
    m_shell = QFile::decodeName(p->pw_shell);
    m_fullName = QString::fromLocal8Bit(p->pw_gecos).section(QLatin1Char(','), 0, 0);
}

bool KUser::isValid() const { return m_valid; }
uid_t KUser::uid() const { return m_valid ? m_uid : uid_t(-1); }
gid_t KUser::gid() const { return m_valid ? m_gid : gid_t(-1); }
bool KUser::isSuperUser() const { return m_valid && m_uid == 0; }
QString KUser::loginName() const { return m_login; }
QString KUser::homeDir() const { return m_home; }
QString KUser::faceIconPath() const { return QString(); }
QString KUser::shell() const { return m_shell; }
QStringList KUser::groupNames() const { return QStringList(); }

QVariant KUser::property(UserProperty which) const
{
    if (which == FullName)
        return m_fullName;
    return QVariant();
}

QList<KUser> KUser::allUsers()
{
    QList<KUser> list;
    list << KUser();
    return list;
}

QStringList KUser::allUserNames()
{
    return QStringList() << KUser().loginName();
}
