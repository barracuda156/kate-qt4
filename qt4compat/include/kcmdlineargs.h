/* qt4compat: kcmdlineargs.h */
#ifndef KDECOMPAT_KCMDLINEARGS_H
#define KDECOMPAT_KCMDLINEARGS_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <kdemacros.h>
#include <klocalizedstring.h>
#include <kurl.h>

class KAboutData;
class KCmdLineArgs;
class KCmdLineArgsPrivate;
class KCmdLineOptionsPrivate;

class KDECORE_EXPORT KCmdLineOptions
{
    friend class KCmdLineArgs;
    friend class KCmdLineArgsPrivate;
public:
    KCmdLineOptions();
    ~KCmdLineOptions();
    KCmdLineOptions(const KCmdLineOptions &options);
    KCmdLineOptions &operator=(const KCmdLineOptions &options);

    KCmdLineOptions &add(const QByteArray &name, const KLocalizedString &description = KLocalizedString(),
                         const QByteArray &defaultValue = QByteArray());
    KCmdLineOptions &add(const KCmdLineOptions &options);

private:
    KCmdLineOptionsPrivate *d;
};

class KDECORE_EXPORT KCmdLineArgs
{
    friend class KCmdLineArgsPrivate;
public:
    enum StdCmdLineArg {
        CmdLineArgQt = 0x01,
        CmdLineArgKDE = 0x02,
        CmdLineArgsMask = 0x03,
        CmdLineArgNone = 0x00,
        Reserved = 0xff
    };
    Q_DECLARE_FLAGS(StdCmdLineArgs, StdCmdLineArg)

    static void init(int argc, char **argv, const KAboutData *about,
                     StdCmdLineArgs stdargs = StdCmdLineArgs(CmdLineArgQt | CmdLineArgKDE));
    static void init(int argc, char **argv, const QByteArray &appname, const QByteArray &catalog,
                     const KLocalizedString &programName, const QByteArray &version,
                     const KLocalizedString &description = KLocalizedString(),
                     StdCmdLineArgs stdargs = StdCmdLineArgs(CmdLineArgQt | CmdLineArgKDE));
    static void init(const KAboutData *about);
    static void addStdCmdLineOptions(StdCmdLineArgs stdargs = StdCmdLineArgs(CmdLineArgQt | CmdLineArgKDE));
    static void addCmdLineOptions(const KCmdLineOptions &options, const KLocalizedString &name = KLocalizedString(),
                                  const QByteArray &id = QByteArray(), const QByteArray &afterId = QByteArray());
    static KCmdLineArgs *parsedArgs(const QByteArray &id = QByteArray());
    static QString cwd();
    static QString appName();
    static void usage(const QByteArray &id = QByteArray());
    static void usageError(const QString &error);
    static void enable_i18n();
    static void addTempFileOption();
    static bool isTempFileSet();
    static int &qtArgc();
    static char **qtArgv();
    static void reset();
    static const KAboutData *aboutData();
    static QStringList allArguments();
    static void setCwd(const QByteArray &cwd);

    QString getOption(const QByteArray &option) const;
    QStringList getOptionList(const QByteArray &option) const;
    bool isSet(const QByteArray &option) const;
    int count() const;
    QString arg(int n) const;
    KUrl url(int n) const;
    static KUrl makeURL(const QByteArray &urlArg);
    void clear();

    ~KCmdLineArgs();

private:
    KCmdLineArgs(const KCmdLineOptions &options, const KLocalizedString &name, const QByteArray &id);
    KCmdLineArgsPrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCmdLineArgs::StdCmdLineArgs)

#endif
