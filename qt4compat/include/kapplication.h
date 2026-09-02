/* qt4compat: kapplication.h - QApplication plus the bits of KApplication Kate needs.
 * Finder "open with" requests (QFileOpenEvent) are re-emitted as openURLRequest(). */
#ifndef KDECOMPAT_KAPPLICATION_H
#define KDECOMPAT_KAPPLICATION_H

#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <kdemacros.h>
#include <ksharedptr.h>

class KConfig;
class KSharedConfig;
typedef KSharedPtr<KSharedConfig> KSharedConfigPtr;

#define kapp KApplication::kApplication()

class KDEUI_EXPORT KApplication : public QApplication
{
    Q_OBJECT
public:
    explicit KApplication(bool GUIenabled = true);
    virtual ~KApplication();

    static KApplication *kApplication();

    KConfig *sessionConfig();
    void disableSessionManagement();
    void enableSessionManagement();
    static void setTopWidget(QWidget *topWidget);
    static bool sessionSaving();
    QString tempSaveName(const QString &pFilename);
    QString checkRecoverFile(const QString &pFilename, bool &bRecover);
    void updateUserTimestamp(int time = 0);
    unsigned long userTimestamp() const;
    void updateRemoteUserTimestamp(const QString &service, int time = 0);
    static void setQuitOnLastWindowClosed(bool quit);

    virtual bool event(QEvent *e);

public Q_SLOTS:
    void quit();

Q_SIGNALS:
    void openURLRequest(const QUrl &url);
    void saveYourself();
    void aboutToQuit();
};

#endif
