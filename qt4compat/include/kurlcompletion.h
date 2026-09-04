/* qt4compat: kurlcompletion.h - local-files-only KUrlCompletion.
 * kdelibs' KUrlCompletion sits on KIO (remote listings, async jobs); Kate on
 * plain Qt 4 has no KIO, so this is a small synchronous replacement that
 * only ever completes paths on the local filesystem via QDir, keeping the
 * kdelibs public API (enum Mode, setDir/dir, setMode/mode, replaceHome,
 * replaceEnv, isRunning, stop()) so callers written against kdelibs (the
 * search plugin's folder requester) compile unmodified. */
#ifndef KDECOMPAT_KURLCOMPLETION_H
#define KDECOMPAT_KURLCOMPLETION_H

#include <kdemacros.h>
#include <kcompletion.h>

class KDEUI_EXPORT KUrlCompletion : public KCompletion
{
public:
    enum Mode { ExeCompletion = 1, FileCompletion = 2, DirCompletion = 3 };

    KUrlCompletion();
    explicit KUrlCompletion(Mode mode);
    virtual ~KUrlCompletion();

    virtual QString makeCompletion(const QString &text);

    void setDir(const QString &dir);
    QString dir() const;

    Mode mode() const;
    void setMode(Mode mode);

    /** always false: makeCompletion() runs synchronously, there is nothing to wait for */
    bool isRunning() const;
    /** no-op: nothing asynchronous ever runs */
    void stop();

    bool replaceHome() const;
    void setReplaceHome(bool replace);

    /** stored but not acted on: no environment-variable expansion is done */
    bool replaceEnv() const;
    void setReplaceEnv(bool replace);

private:
    QString expandTilde(const QString &text) const;

    Mode m_mode;
    QString m_dir;
    bool m_replaceHome;
    bool m_replaceEnv;
};

#endif
