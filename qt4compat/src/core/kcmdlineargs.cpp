/* qt4compat: kcmdlineargs.cpp - KDE4-style option parsing on plain argv */
#include <kcmdlineargs.h>

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kdeversion.h>
#include <kglobal.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QTextStream>

#include <cstdio>
#include <cstdlib>

namespace {

struct KOptionDef
{
    QByteArray name;        /* long name without dashes, or "+name" for positional */
    QByteArray argName;     /* non-empty when the option takes a value */
    QByteArray defaultValue;
    KLocalizedString description;
    bool positional;
    bool optional;          /* for "+[name]" */
    bool negatable;         /* "noxxx" style option: default true */
    QByteArray aliasOf;     /* short form aliases the next option */
};

struct KOptionValue
{
    bool set;
    QStringList values;
    KOptionValue() : set(false) {}
};

} // namespace

class KCmdLineOptionsPrivate
{
public:
    QList<KOptionDef> defs;
};

class KCmdLineArgsPrivate
{
public:
    KCmdLineArgsPrivate(const KCmdLineOptions &o, const KLocalizedString &n, const QByteArray &i)
        : options(o), name(n), id(i), parsed(false) {}
    KCmdLineOptions options;
    KLocalizedString name;
    QByteArray id;
    bool parsed;
    QHash<QByteArray, KOptionValue> values;
    QStringList positional;

    const KOptionDef *find(const QByteArray &name) const;
    static void parseAll();
    static void printUsageAndExit(int code);
};

namespace {

struct KCmdLineState
{
    KCmdLineState() : argc(0), argv(0), about(0), ownAbout(0), tempFileOption(false), parsed(false) {}
    int argc;
    char **argv;
    QStringList args;   /* copy of argv[1..] as given to init() */
    const KAboutData *about;
    KAboutData *ownAbout;
    QList<KCmdLineArgs *> argsList;
    QString cwd;
    bool tempFileOption;
    bool parsed;
};

static KCmdLineState *state()
{
    static KCmdLineState s;
    return &s;
}

static bool isQtOption(const QString &name, bool *takesValue)
{
    static const char *withValue[] = {
        "style", "stylesheet", "session", "graphicssystem", "display", "geometry", "fn", "font",
        "bg", "background", "fg", "foreground", "btn", "button", "name", "title", "visual",
        "ncols", "cmap", "im", "inputstyle", "qmljsdebugger", "qws", "qwindowgeometry",
        "qwindowicon", "qwindowtitle", "display", "platform", 0
    };
    static const char *flags[] = {
        "widgetcount", "reverse", "sync", "nograb", "dograb", "noxim", "disable-nls", 0
    };
    for (int i = 0; withValue[i]; ++i) {
        if (name == QLatin1String(withValue[i])) {
            *takesValue = true;
            return true;
        }
    }
    for (int i = 0; flags[i]; ++i) {
        if (name == QLatin1String(flags[i])) {
            *takesValue = false;
            return true;
        }
    }
    return false;
}

} // namespace

/* KCmdLineOptions */

KCmdLineOptions::KCmdLineOptions()
    : d(new KCmdLineOptionsPrivate)
{
}

KCmdLineOptions::~KCmdLineOptions()
{
    delete d;
}

KCmdLineOptions::KCmdLineOptions(const KCmdLineOptions &options)
    : d(new KCmdLineOptionsPrivate(*options.d))
{
}

KCmdLineOptions &KCmdLineOptions::operator=(const KCmdLineOptions &options)
{
    if (this != &options)
        *d = *options.d;
    return *this;
}

KCmdLineOptions &KCmdLineOptions::add(const QByteArray &name, const KLocalizedString &description, const QByteArray &defaultValue)
{
    KOptionDef def;
    def.positional = false;
    def.optional = false;
    def.negatable = false;
    def.description = description;
    def.defaultValue = defaultValue;

    QByteArray n = name.trimmed();
    if (n.startsWith('!'))
        n = n.mid(1);
    if (n.startsWith('+')) {
        def.positional = true;
        n = n.mid(1);
        if (n.startsWith('[') && n.endsWith(']')) {
            def.optional = true;
            n = n.mid(1, n.length() - 2);
        }
        def.name = n;
    } else {
        const int space = n.indexOf(' ');
        if (space > 0) {
            def.argName = n.mid(space + 1).trimmed();
            n = n.left(space);
        }
        if (n.startsWith(':')) {
            /* section header, not an option */
            return *this;
        }
        if (n.startsWith("no") && def.argName.isEmpty() && n.length() > 2) {
            def.negatable = true;
            n = n.mid(2);
        }
        def.name = n;
    }
    d->defs.append(def);
    return *this;
}

KCmdLineOptions &KCmdLineOptions::add(const KCmdLineOptions &options)
{
    d->defs += options.d->defs;
    return *this;
}

/* KCmdLineArgs */

KCmdLineArgs::KCmdLineArgs(const KCmdLineOptions &options, const KLocalizedString &name, const QByteArray &id)
    : d(new KCmdLineArgsPrivate(options, name, id))
{
    /* resolve aliases: an option without description directly followed by another one */
    QList<KOptionDef> &defs = d->options.d->defs;
    for (int i = 0; i + 1 < defs.count(); ++i) {
        if (!defs[i].positional && defs[i].description.isEmpty() && defs[i].argName.isEmpty() && defs[i].aliasOf.isEmpty()) {
            defs[i].aliasOf = defs[i + 1].name;
        }
    }
}

KCmdLineArgs::~KCmdLineArgs()
{
    delete d;
}

const KOptionDef *KCmdLineArgsPrivate::find(const QByteArray &name) const
{
    const QList<KOptionDef> &defs = options.d->defs;
    for (int i = 0; i < defs.count(); ++i) {
        if (defs[i].positional)
            continue;
        if (defs[i].name == name) {
            if (!defs[i].aliasOf.isEmpty())
                return find(defs[i].aliasOf);
            return &defs[i];
        }
    }
    return 0;
}

void KCmdLineArgsPrivate::parseAll()
{
    KCmdLineState *s = state();
    if (s->parsed)
        return;
    s->parsed = true;

    KCmdLineArgs *positionalOwner = 0;
    foreach (KCmdLineArgs *a, s->argsList) {
        foreach (const KOptionDef &def, a->d->options.d->defs) {
            if (def.positional) {
                positionalOwner = a;
                break;
            }
        }
        if (positionalOwner)
            break;
    }

    bool onlyPositional = false;
    const QStringList &args = s->args;
    for (int i = 0; i < args.count(); ++i) {
        const QString arg = args.at(i);
        if (onlyPositional || !arg.startsWith(QLatin1Char('-')) || arg == QLatin1String("-")) {
            if (positionalOwner)
                positionalOwner->d->positional << arg;
            continue;
        }
        if (arg == QLatin1String("--")) {
            onlyPositional = true;
            continue;
        }
        QString name = arg;
        while (name.startsWith(QLatin1Char('-')))
            name = name.mid(1);
        QString value;
        bool hasValue = false;
        const int eq = name.indexOf(QLatin1Char('='));
        if (eq >= 0) {
            value = name.mid(eq + 1);
            name = name.left(eq);
            hasValue = true;
        }
        if (name.startsWith(QLatin1String("psn_")))
            continue; /* macOS process serial number */
        if (name == QLatin1String("help") || name == QLatin1String("h") || name.startsWith(QLatin1String("help-")))
            KCmdLineArgsPrivate::printUsageAndExit(0);
        if (name == QLatin1String("version") || name == QLatin1String("v")) {
            QTextStream out(stdout);
            if (s->about)
                out << s->about->programName() << ' ' << s->about->version() << '\n';
            out << "Qt: " << qVersion() << '\n';
            out.flush();
            ::exit(0);
        }
        if (name == QLatin1String("author") || name == QLatin1String("license")) {
            QTextStream out(stdout);
            if (s->about) {
                if (name == QLatin1String("author")) {
                    foreach (const KAboutPerson &p, s->about->authors())
                        out << p.name() << (p.emailAddress().isEmpty() ? QString() : QLatin1String(" <") + p.emailAddress() + QLatin1Char('>')) << '\n';
                } else {
                    out << s->about->license() << '\n';
                }
            }
            out.flush();
            ::exit(0);
        }
        bool qtTakesValue = false;
        if (isQtOption(name, &qtTakesValue)) {
            if (qtTakesValue && !hasValue)
                ++i;
            continue;
        }
        if (name == QLatin1String("tempfile")) {
            s->tempFileOption = true;
            continue;
        }

        /* find the owner */
        const KOptionDef *def = 0;
        KCmdLineArgs *owner = 0;
        bool negated = false;
        foreach (KCmdLineArgs *a, s->argsList) {
            def = a->d->find(name.toUtf8());
            if (def) {
                owner = a;
                break;
            }
        }
        if (!def && name.startsWith(QLatin1String("no"))) {
            foreach (KCmdLineArgs *a, s->argsList) {
                def = a->d->find(name.mid(2).toUtf8());
                if (def && def->argName.isEmpty()) {
                    owner = a;
                    negated = true;
                    break;
                }
                def = 0;
            }
        }
        if (!def) {
            fprintf(stderr, "%s: Unknown option '%s'.\n", qPrintable(KCmdLineArgs::appName()), qPrintable(arg));
            continue;
        }
        KOptionValue &v = owner->d->values[def->name];
        if (!def->argName.isEmpty()) {
            if (!hasValue) {
                if (i + 1 < args.count()) {
                    value = args.at(++i);
                } else {
                    fprintf(stderr, "%s: '%s' missing.\n", qPrintable(KCmdLineArgs::appName()), qPrintable(arg));
                    continue;
                }
            }
            v.set = true;
            v.values << value;
        } else {
            v.set = true;
            v.values = QStringList() << (negated ? QLatin1String("false") : QLatin1String("true"));
        }
    }
}

void KCmdLineArgsPrivate::printUsageAndExit(int code)
{
    KCmdLineState *s = state();
    QTextStream out(stdout);
    out << "Usage: " << KCmdLineArgs::appName() << " [options]";
    foreach (KCmdLineArgs *a, s->argsList) {
        foreach (const KOptionDef &def, a->d->options.d->defs) {
            if (def.positional)
                out << ' ' << (def.optional ? "[" : "") << QString::fromUtf8(def.name) << (def.optional ? "]" : "");
        }
    }
    out << "\n\n";
    if (s->about && !s->about->shortDescription().isEmpty())
        out << s->about->shortDescription() << "\n\n";
    foreach (KCmdLineArgs *a, s->argsList) {
        if (!a->d->name.isEmpty())
            out << a->d->name.toString() << ":\n";
        const QList<KOptionDef> &defs = a->d->options.d->defs;
        for (int i = 0; i < defs.count(); ++i) {
            const KOptionDef &def = defs[i];
            QString line;
            if (def.positional) {
                line = QLatin1String("  ") + QString::fromUtf8(def.name);
            } else if (!def.aliasOf.isEmpty()) {
                line = QLatin1String("  -") + QString::fromUtf8(def.name) + QLatin1String(", ");
                if (i + 1 < defs.count()) {
                    const KOptionDef &next = defs[i + 1];
                    line += QLatin1String("--") + QString::fromUtf8(next.name);
                    if (!next.argName.isEmpty())
                        line += QLatin1String(" <") + QString::fromUtf8(next.argName) + QLatin1Char('>');
                    ++i;
                    line = line.leftJustified(34) + next.description.toString();
                    out << line << '\n';
                    continue;
                }
            } else {
                line = QLatin1String("  --") + QString::fromUtf8(def.name);
                if (!def.argName.isEmpty())
                    line += QLatin1String(" <") + QString::fromUtf8(def.argName) + QLatin1Char('>');
            }
            line = line.leftJustified(34) + def.description.toString();
            out << line << '\n';
        }
        out << '\n';
    }
    out.flush();
    ::exit(code);
}

void KCmdLineArgs::init(int argc, char **argv, const KAboutData *about, StdCmdLineArgs)
{
    KCmdLineState *s = state();
    s->argc = argc;
    s->argv = argv;
    s->about = about;
    s->args.clear();
    for (int i = 1; i < argc; ++i)
        s->args << QString::fromLocal8Bit(argv[i]);
    s->cwd = QDir::currentPath();
    s->parsed = false;
    if (about && !KGlobal::hasMainComponent()) {
        /* registers itself as the main component */
        KComponentData data(about);
        Q_UNUSED(data)
    }
}

void KCmdLineArgs::init(int argc, char **argv, const QByteArray &appname, const QByteArray &catalog,
                        const KLocalizedString &programName, const QByteArray &version,
                        const KLocalizedString &description, StdCmdLineArgs stdargs)
{
    KCmdLineState *s = state();
    delete s->ownAbout;
    s->ownAbout = new KAboutData(appname, catalog, programName, version, description);
    init(argc, argv, s->ownAbout, stdargs);
}

void KCmdLineArgs::init(const KAboutData *about)
{
    static char *fakeArgv[] = { const_cast<char *>("kate"), 0 };
    init(1, fakeArgv, about);
}

void KCmdLineArgs::addStdCmdLineOptions(StdCmdLineArgs)
{
}

void KCmdLineArgs::addCmdLineOptions(const KCmdLineOptions &options, const KLocalizedString &name, const QByteArray &id, const QByteArray &)
{
    KCmdLineState *s = state();
    foreach (KCmdLineArgs *a, s->argsList) {
        if (a->d->id == id) {
            a->d->options.add(options);
            return;
        }
    }
    s->argsList.append(new KCmdLineArgs(options, name, id));
}

KCmdLineArgs *KCmdLineArgs::parsedArgs(const QByteArray &id)
{
    KCmdLineState *s = state();
    KCmdLineArgsPrivate::parseAll();
    foreach (KCmdLineArgs *a, s->argsList) {
        if (a->d->id == id)
            return a;
    }
    if (id.isEmpty() && !s->argsList.isEmpty())
        return s->argsList.first();
    /* no options registered: give an empty instance so callers can query safely */
    static KCmdLineArgs *empty = 0;
    if (!empty)
        empty = new KCmdLineArgs(KCmdLineOptions(), KLocalizedString(), id);
    return empty;
}

QString KCmdLineArgs::cwd()
{
    return state()->cwd;
}

QString KCmdLineArgs::appName()
{
    KCmdLineState *s = state();
    if (s->about)
        return s->about->appName();
    if (s->argc > 0 && s->argv)
        return QFileInfo(QString::fromLocal8Bit(s->argv[0])).fileName();
    return QLatin1String("kate");
}

void KCmdLineArgs::usage(const QByteArray &)
{
    KCmdLineArgsPrivate::printUsageAndExit(0);
}

void KCmdLineArgs::usageError(const QString &error)
{
    fprintf(stderr, "%s: %s\n", qPrintable(appName()), qPrintable(error));
    fprintf(stderr, "Use --help to get a list of available command line options.\n");
    ::exit(254);
}

void KCmdLineArgs::enable_i18n()
{
}

void KCmdLineArgs::addTempFileOption()
{
    KCmdLineOptions options;
    options.add("tempfile", ki18n("The files/URLs opened by the application will be deleted after use"));
    addCmdLineOptions(options, ki18n("KDE-tempfile"), "kde-tempfile");
}

bool KCmdLineArgs::isTempFileSet()
{
    KCmdLineArgsPrivate::parseAll();
    return state()->tempFileOption;
}

int &KCmdLineArgs::qtArgc()
{
    return state()->argc;
}

char **KCmdLineArgs::qtArgv()
{
    return state()->argv;
}

void KCmdLineArgs::reset()
{
    KCmdLineState *s = state();
    qDeleteAll(s->argsList);
    s->argsList.clear();
    s->parsed = false;
}

const KAboutData *KCmdLineArgs::aboutData()
{
    return state()->about;
}

QStringList KCmdLineArgs::allArguments()
{
    KCmdLineState *s = state();
    QStringList all;
    if (s->argc > 0 && s->argv)
        all << QString::fromLocal8Bit(s->argv[0]);
    all << s->args;
    return all;
}

void KCmdLineArgs::setCwd(const QByteArray &cwd)
{
    state()->cwd = QString::fromLocal8Bit(cwd);
}

QString KCmdLineArgs::getOption(const QByteArray &option) const
{
    KCmdLineArgsPrivate::parseAll();
    const KOptionDef *def = d->find(option);
    const QByteArray key = def ? def->name : option;
    if (d->values.contains(key) && !d->values.value(key).values.isEmpty())
        return d->values.value(key).values.last();
    if (def) {
        if (!def->argName.isEmpty())
            return QString::fromUtf8(def->defaultValue);
        return def->negatable ? QLatin1String("true") : QLatin1String("false");
    }
    return QString();
}

QStringList KCmdLineArgs::getOptionList(const QByteArray &option) const
{
    KCmdLineArgsPrivate::parseAll();
    const KOptionDef *def = d->find(option);
    const QByteArray key = def ? def->name : option;
    if (d->values.contains(key))
        return d->values.value(key).values;
    return QStringList();
}

bool KCmdLineArgs::isSet(const QByteArray &option) const
{
    KCmdLineArgsPrivate::parseAll();
    const KOptionDef *def = d->find(option);
    const QByteArray key = def ? def->name : option;
    if (d->values.contains(key)) {
        const KOptionValue &v = d->values.value(key);
        if (def && def->argName.isEmpty())
            return v.values.isEmpty() || v.values.last() != QLatin1String("false");
        return v.set;
    }
    if (def && def->argName.isEmpty())
        return def->negatable;
    if (def && !def->argName.isEmpty())
        return !def->defaultValue.isEmpty();
    return false;
}

int KCmdLineArgs::count() const
{
    KCmdLineArgsPrivate::parseAll();
    return d->positional.count();
}

QString KCmdLineArgs::arg(int n) const
{
    KCmdLineArgsPrivate::parseAll();
    return d->positional.value(n);
}

KUrl KCmdLineArgs::url(int n) const
{
    return makeURL(arg(n).toUtf8());
}

KUrl KCmdLineArgs::makeURL(const QByteArray &urlArg)
{
    const QString urlStr = QString::fromUtf8(urlArg);
    if (urlStr.isEmpty())
        return KUrl();
    if (QDir::isAbsolutePath(urlStr) || urlStr.startsWith(QLatin1Char('~')))
        return KUrl::fromPath(QDir::cleanPath(KUrl(urlStr).toLocalFile()));
    if (!KUrl::isRelativeUrl(urlStr)) {
        KUrl u(urlStr);
        if (u.isValid() && !u.scheme().isEmpty())
            return u;
    }
    /* relative path: resolve against the current directory */
    const QString base = state()->cwd.isEmpty() ? QDir::currentPath() : state()->cwd;
    return KUrl::fromPath(QDir::cleanPath(base + QLatin1Char('/') + urlStr));
}

void KCmdLineArgs::clear()
{
    d->values.clear();
    d->positional.clear();
}
