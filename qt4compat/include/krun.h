/* qt4compat: krun.h - opens URLs with the desktop's default handler */
#ifndef KDECOMPAT_KRUN_H
#define KDECOMPAT_KRUN_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <kservice.h>
#include <kurl.h>

#include <sys/types.h>

class QWidget;
class KRunPrivate;

class KIO_EXPORT KRun : public QObject
{
    Q_OBJECT
public:
    KRun(const KUrl &url, QWidget *window, mode_t mode = 0, bool isLocalFile = false,
         bool showProgressInfo = true, const QByteArray &asn = QByteArray());
    virtual ~KRun();

    void abort();
    bool hasError() const;
    bool hasFinished() const;
    bool autoDelete() const;
    void setAutoDelete(bool b);
    void setPreferredService(const QString &desktopEntryName);
    void setRunExecutables(bool b);
    void setEnableExternalBrowser(bool b);
    void setSuggestedFileName(const QString &fileName);
    QString suggestedFileName() const;
    KUrl url() const;

    static bool run(const KService &service, const KUrl::List &urls, QWidget *window,
                    bool tempFiles = false, const QString &suggestedFileName = QString(),
                    const QByteArray &asn = QByteArray());
    static bool run(const QString &exec, const KUrl::List &urls, QWidget *window,
                    const QString &name = QString(), const QString &icon = QString(),
                    const QByteArray &asn = QByteArray());
    static bool runUrl(const KUrl &url, const QString &mimetype, QWidget *window,
                       bool tempFile = false, bool runExecutables = true,
                       const QString &suggestedFileName = QString(), const QByteArray &asn = QByteArray());
    static bool runCommand(const QString &cmd, QWidget *window);
    static bool runCommand(const QString &cmd, QWidget *window, const QString &workingDirectory);
    static bool runCommand(const QString &cmd, const QString &execName, const QString &icon,
                           QWidget *window, const QByteArray &asn = QByteArray());
    static bool runCommand(const QString &cmd, const QString &execName, const QString &icon,
                           QWidget *window, const QByteArray &asn, const QString &workingDirectory);
    static bool displayOpenWithDialog(const KUrl::List &lst, QWidget *window, bool tempFiles = false,
                                      const QString &suggestedFileName = QString(), const QByteArray &asn = QByteArray());
    static void shellQuote(QString &str);
    static QStringList processDesktopExec(const KService &service, const KUrl::List &urls, bool tempFiles = false,
                                          const QString &suggestedFileName = QString());
    static QString binaryName(const QString &execLine, bool removePath);
    static bool isExecutable(const QString &serviceType);
    static bool isExecutableFile(const KUrl &url, const QString &mimetype);

Q_SIGNALS:
    void finished();
    void error();

protected Q_SLOTS:
    void slotStart();

private:
    KRunPrivate *const d;
};

#endif
