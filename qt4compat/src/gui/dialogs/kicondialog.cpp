/* qt4compat: kicondialog.cpp */
#include <kicondialog.h>

#include <kfiledialog.h>
#include <kicon.h>
#include <klocale.h>

#include <QtCore/QFileInfo>

QString KIconDialog::getIcon(KIconLoader::Group, KIconLoader::Context, bool, int, bool, QWidget *parent, const QString &caption)
{
    const QString file = KFileDialog::getOpenFileName(KUrl(), QLatin1String("*.png *.svg *.svgz *.xpm *.icns|") + i18n("Icons"),
                                                      parent, caption.isEmpty() ? i18n("Select Icon") : caption);
    return file;
}

KIconButton::KIconButton(QWidget *parent)
    : QPushButton(parent), m_size(0), m_strict(false)
{
    connect(this, SIGNAL(clicked()), this, SLOT(slotChangeIcon()));
}

KIconButton::KIconButton(KIconLoader *, QWidget *parent)
    : QPushButton(parent), m_size(0), m_strict(false)
{
    connect(this, SIGNAL(clicked()), this, SLOT(slotChangeIcon()));
}

KIconButton::~KIconButton()
{
}

void KIconButton::setStrictIconSize(bool b)
{
    m_strict = b;
}

bool KIconButton::strictIconSize() const
{
    return m_strict;
}

void KIconButton::setIconType(KIconLoader::Group, KIconLoader::Context, bool)
{
}

void KIconButton::setIcon(const QString &icon)
{
    m_icon = icon;
    QPushButton::setIcon(KIcon(icon));
    emit iconChanged(icon);
}

void KIconButton::setIcon(const QIcon &icon)
{
    m_icon.clear();
    QPushButton::setIcon(icon);
}

void KIconButton::resetIcon()
{
    m_icon.clear();
    QPushButton::setIcon(QIcon());
    emit iconChanged(QString());
}

const QString &KIconButton::icon() const
{
    return m_icon;
}

void KIconButton::setIconSize(int size)
{
    m_size = size;
}

int KIconButton::iconSize() const
{
    return m_size;
}

void KIconButton::setButtonIconSize(int size)
{
    QPushButton::setIconSize(QSize(size, size));
}

int KIconButton::buttonIconSize() const
{
    return QPushButton::iconSize().width();
}

void KIconButton::slotChangeIcon()
{
    const QString file = KIconDialog::getIcon(KIconLoader::Desktop, KIconLoader::Application, m_strict, m_size, false, this);
    if (!file.isEmpty())
        setIcon(file);
}
