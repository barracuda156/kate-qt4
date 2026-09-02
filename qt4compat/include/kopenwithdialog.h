/* qt4compat: kopenwithdialog.h - there is no application database; the dialog only explains that */
#ifndef KDECOMPAT_KOPENWITHDIALOG_H
#define KDECOMPAT_KOPENWITHDIALOG_H

#include <QtCore/QString>
#include <kdemacros.h>
#include <kdialog.h>
#include <kservice.h>
#include <kurl.h>

class KDEUI_EXPORT KOpenWithDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KOpenWithDialog(const KUrl::List &urls, QWidget *parent = 0);
    KOpenWithDialog(const KUrl::List &urls, const QString &text, const QString &value, QWidget *parent = 0);
    KOpenWithDialog(const QString &mimeType, const QString &value, QWidget *parent = 0);
    explicit KOpenWithDialog(QWidget *parent = 0);
    ~KOpenWithDialog();

    QString text() const;
    void hideNoCloseOnExit();
    void hideRunInTerminal();
    KService::Ptr service() const;
    void setSaveNewApplications(bool b);

public Q_SLOTS:
    virtual void accept();

private:
    void init(const QString &text, const QString &value);
    QString m_text;
};

#endif
