/* qt4compat: kicondialog.h - KIconButton lets the user pick an icon file; KIconDialog is a file picker */
#ifndef KDECOMPAT_KICONDIALOG_H
#define KDECOMPAT_KICONDIALOG_H

#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QPushButton>
#include <kdemacros.h>
#include <kiconloader.h>

class KDEUI_EXPORT KIconDialog
{
public:
    static QString getIcon(KIconLoader::Group group = KIconLoader::Desktop, KIconLoader::Context context = KIconLoader::Application,
                           bool strictIconSize = false, int iconSize = 0, bool user = false, QWidget *parent = 0,
                           const QString &caption = QString());
};

class KDEUI_EXPORT KIconButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString icon READ icon WRITE setIcon RESET resetIcon)
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(bool strictIconSize READ strictIconSize WRITE setStrictIconSize)
public:
    explicit KIconButton(QWidget *parent = 0);
    KIconButton(KIconLoader *loader, QWidget *parent);
    ~KIconButton();

    void setStrictIconSize(bool b);
    bool strictIconSize() const;
    void setIconType(KIconLoader::Group group, KIconLoader::Context context, bool user = false);
    void setIcon(const QString &icon);
    void setIcon(const QIcon &icon);
    void resetIcon();
    const QString &icon() const;
    void setIconSize(int size);
    int iconSize() const;
    void setButtonIconSize(int size);
    int buttonIconSize() const;

Q_SIGNALS:
    void iconChanged(const QString &icon);

private Q_SLOTS:
    void slotChangeIcon();

private:
    QString m_icon;
    int m_size;
    bool m_strict;
};

#endif
