/* qt4compat: kfiledialog.cpp */
#include <kfiledialog.h>

#include <kconfiggroup.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksharedconfig.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>

static QString s_lastDir;

/* "*.txt *.text|Text files\n*|All files" -> "Text files (*.txt *.text);;All files (*)" */
QString KFileDialog::qtFilter(const QString &kdeFilter)
{
    if (kdeFilter.isEmpty())
        return QString();
    if (kdeFilter.contains(QLatin1Char('(')) && !kdeFilter.contains(QLatin1Char('|')))
        return kdeFilter; /* already Qt style */
    QStringList result;
    foreach (const QString &line, kdeFilter.split(QLatin1Char('\n'), QString::SkipEmptyParts)) {
        const int bar = line.indexOf(QLatin1Char('|'));
        QString pattern = bar < 0 ? line : line.left(bar);
        QString label = bar < 0 ? QString() : line.mid(bar + 1);
        pattern = pattern.trimmed();
        if (pattern.isEmpty())
            continue;
        if (pattern.contains(QLatin1Char('/')) && !pattern.contains(QLatin1Char('*'))) {
            /* a mime type: no way to map it without a database, accept everything */
            if (label.isEmpty())
                label = pattern;
            pattern = QLatin1String("*");
        }
        if (label.isEmpty())
            label = pattern;
        result << label + QLatin1String(" (") + pattern + QLatin1Char(')');
    }
    return result.join(QLatin1String(";;"));
}

KUrl KFileDialog::getStartUrl(const KUrl &startDir, QString &recentDirClass)
{
    recentDirClass.clear();
    if (startDir.isEmpty() || !startDir.isValid()) {
        if (!s_lastDir.isEmpty())
            return KUrl::fromPath(s_lastDir);
        return KUrl::fromPath(QDir::homePath());
    }
    const QString str = startDir.url();
    if (str.startsWith(QLatin1String("kfiledialog:///")) || startDir.protocol() == QLatin1String("kfiledialog")) {
        /* ":label" / "kfiledialog:///label" recent-dir syntax */
        QString label = startDir.path();
        while (label.startsWith(QLatin1Char('/')))
            label = label.mid(1);
        recentDirClass = label.section(QLatin1Char('/'), 0, 0);
        KConfigGroup cg(KGlobal::config(), "Recent Dirs");
        const QString dir = cg.readPathEntry(recentDirClass, QString());
        if (!dir.isEmpty() && QDir(dir).exists())
            return KUrl::fromPath(dir);
        return KUrl::fromPath(QDir::homePath());
    }
    if (startDir.isLocalFile()) {
        const QString path = startDir.toLocalFile();
        if (path.startsWith(QLatin1Char(':'))) {
            recentDirClass = path.mid(1);
            KConfigGroup cg(KGlobal::config(), "Recent Dirs");
            const QString dir = cg.readPathEntry(recentDirClass, QString());
            if (!dir.isEmpty() && QDir(dir).exists())
                return KUrl::fromPath(dir);
            return KUrl::fromPath(QDir::homePath());
        }
        return startDir;
    }
    return KUrl::fromPath(QDir::homePath());
}

void KFileDialog::setStartDir(const KUrl &directory)
{
    if (directory.isLocalFile())
        s_lastDir = directory.toLocalFile();
}

void KFileDialog::rememberDir(const QString &recentDirClass, const QString &path)
{
    if (path.isEmpty())
        return;
    const QFileInfo fi(path);
    const QString dir = fi.isDir() ? fi.absoluteFilePath() : fi.absolutePath();
    s_lastDir = dir;
    if (!recentDirClass.isEmpty()) {
        KConfigGroup cg(KGlobal::config(), "Recent Dirs");
        cg.writePathEntry(recentDirClass, dir);
        cg.sync();
    }
}

static QString startPath(const KUrl &startDir, QString &recentDirClass)
{
    const KUrl url = KFileDialog::getStartUrl(startDir, recentDirClass);
    QString path = url.toLocalFile();
    if (path.isEmpty())
        path = QDir::homePath();
    return path;
}

QString KFileDialog::getOpenFileName(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    QString cls;
    const QString path = startPath(startDir, cls);
    const QString f = QFileDialog::getOpenFileName(parent, caption.isEmpty() ? i18n("Open") : caption, path, qtFilter(filter));
    if (!f.isEmpty())
        rememberDir(cls, f);
    return f;
}

QStringList KFileDialog::getOpenFileNames(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    QString cls;
    const QString path = startPath(startDir, cls);
    const QStringList f = QFileDialog::getOpenFileNames(parent, caption.isEmpty() ? i18n("Open") : caption, path, qtFilter(filter));
    if (!f.isEmpty())
        rememberDir(cls, f.first());
    return f;
}

KUrl KFileDialog::getOpenUrl(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    const QString f = getOpenFileName(startDir, filter, parent, caption);
    return f.isEmpty() ? KUrl() : KUrl::fromPath(f);
}

KUrl::List KFileDialog::getOpenUrls(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    KUrl::List urls;
    foreach (const QString &f, getOpenFileNames(startDir, filter, parent, caption))
        urls.append(KUrl::fromPath(f));
    return urls;
}

QString KFileDialog::getSaveFileName(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    return getSaveFileName(startDir, filter, parent, caption, Options(ConfirmOverwrite));
}

QString KFileDialog::getSaveFileName(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption, Options options)
{
    QString cls;
    QString path;
    if (startDir.isLocalFile() && !QFileInfo(startDir.toLocalFile()).isDir() && !startDir.toLocalFile().startsWith(QLatin1Char(':'))) {
        /* a suggested file name, possibly relative */
        const QString suggested = startDir.toLocalFile();
        if (QDir::isAbsolutePath(suggested))
            path = suggested;
        else
            path = (s_lastDir.isEmpty() ? QDir::homePath() : s_lastDir) + QLatin1Char('/') + suggested;
    } else if (!startDir.isEmpty() && !startDir.isValid()) {
        path = (s_lastDir.isEmpty() ? QDir::homePath() : s_lastDir) + QLatin1Char('/') + startDir.url();
    } else {
        path = startPath(startDir, cls);
    }
    QFileDialog::Options qopts = 0;
    if (!(options & ConfirmOverwrite))
        qopts |= QFileDialog::DontConfirmOverwrite;
    const QString f = QFileDialog::getSaveFileName(parent, caption.isEmpty() ? i18n("Save As") : caption, path, qtFilter(filter), 0, qopts);
    if (!f.isEmpty())
        rememberDir(cls, f);
    return f;
}

KUrl KFileDialog::getSaveUrl(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption)
{
    const QString f = getSaveFileName(startDir, filter, parent, caption);
    return f.isEmpty() ? KUrl() : KUrl::fromPath(f);
}

KUrl KFileDialog::getSaveUrl(const KUrl &startDir, const QString &filter, QWidget *parent, const QString &caption, Options options)
{
    const QString f = getSaveFileName(startDir, filter, parent, caption, options);
    return f.isEmpty() ? KUrl() : KUrl::fromPath(f);
}

QString KFileDialog::getExistingDirectory(const KUrl &startDir, QWidget *parent, const QString &caption)
{
    QString cls;
    const QString path = startPath(startDir, cls);
    const QString d = QFileDialog::getExistingDirectory(parent, caption.isEmpty() ? i18n("Select Folder") : caption, path);
    if (!d.isEmpty())
        rememberDir(cls, d);
    return d;
}

KUrl KFileDialog::getExistingDirectoryUrl(const KUrl &startDir, QWidget *parent, const QString &caption)
{
    const QString d = getExistingDirectory(startDir, parent, caption);
    return d.isEmpty() ? KUrl() : KUrl::fromPath(d);
}

KUrl KFileDialog::getImageOpenUrl(const KUrl &startDir, QWidget *parent, const QString &caption)
{
    return getOpenUrl(startDir, QLatin1String("*.png *.jpg *.jpeg *.gif *.bmp *.xpm|") + i18n("Images"), parent, caption);
}
