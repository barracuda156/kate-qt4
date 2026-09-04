/* qt4compat: kurlcompletion.cpp - see kurlcompletion.h */
#include "kurlcompletion.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLatin1Char>
#include <QtCore/QLatin1String>

KUrlCompletion::KUrlCompletion()
    : KCompletion(), m_mode(FileCompletion), m_replaceHome(true), m_replaceEnv(true)
{
    setOrder(Sorted);
}

KUrlCompletion::KUrlCompletion(Mode mode)
    : KCompletion(), m_mode(mode), m_replaceHome(true), m_replaceEnv(true)
{
    setOrder(Sorted);
}

KUrlCompletion::~KUrlCompletion()
{
}

void KUrlCompletion::setDir(const QString &dir)
{
    m_dir = dir;
}

QString KUrlCompletion::dir() const
{
    return m_dir;
}

KUrlCompletion::Mode KUrlCompletion::mode() const
{
    return m_mode;
}

void KUrlCompletion::setMode(Mode mode)
{
    m_mode = mode;
}

bool KUrlCompletion::isRunning() const
{
    return false;
}

void KUrlCompletion::stop()
{
}

bool KUrlCompletion::replaceHome() const
{
    return m_replaceHome;
}

void KUrlCompletion::setReplaceHome(bool replace)
{
    m_replaceHome = replace;
}

bool KUrlCompletion::replaceEnv() const
{
    return m_replaceEnv;
}

void KUrlCompletion::setReplaceEnv(bool replace)
{
    m_replaceEnv = replace;
}

QString KUrlCompletion::expandTilde(const QString &text) const
{
    if (!m_replaceHome) {
        return text;
    }
    if (text == QLatin1String("~")) {
        return QDir::homePath();
    }
    if (text.startsWith(QLatin1String("~/"))) {
        return QDir::homePath() + text.mid(1);
    }
    return text;
}

QString KUrlCompletion::makeCompletion(const QString &text)
{
    // split into "directory the user already typed" and "prefix being typed"
    const int slash = text.lastIndexOf(QLatin1Char('/'));
    const QString dirPart = (slash >= 0) ? text.left(slash + 1) : QString();
    const QString prefix  = (slash >= 0) ? text.mid(slash + 1) : text;

    // the directory we actually list on disk may differ (~ expansion,
    // relative to the requester's base dir), but candidates are built from
    // the literal dirPart the user typed, so completions still read "~/..."
    QString listDir = expandTilde(dirPart);
    if (listDir.isEmpty()) {
        listDir = m_dir;
    } else if (QDir::isRelativePath(listDir) && !m_dir.isEmpty()) {
        listDir = m_dir + QLatin1Char('/') + listDir;
    }
    if (listDir.isEmpty()) {
        listDir = QDir::currentPath();
    }

    QStringList items;
    QDir dir(listDir);
    if (dir.exists()) {
        const bool showHidden = prefix.startsWith(QLatin1Char('.'));

        QDir::Filters filters = QDir::NoDotAndDotDot;
        filters |= (m_mode == DirCompletion) ? QDir::Dirs : (QDir::Dirs | QDir::Files);
        if (showHidden) {
            filters |= QDir::Hidden;
        }

        const QStringList entries = dir.entryList(filters, QDir::Name);
        for (int i = 0; i < entries.size(); ++i) {
            const QString &name = entries.at(i);
            if (!prefix.isEmpty() && !name.startsWith(prefix, Qt::CaseSensitive)) {
                continue;
            }
            QString candidate = dirPart + name;
            if (QFileInfo(dir, name).isDir()) {
                candidate += QLatin1Char('/');
            }
            items << candidate;
        }
    }

    setItems(items);
    return KCompletion::makeCompletion(text);
}
