/* qt4compat: sonnet/configwidget.cpp */
#include <sonnet/configwidget.h>
#include <sonnet/configdialog.h>

#include <klocalizedstring.h>

#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

namespace Sonnet {

class ConfigWidget::Private
{
public:
    Private() : backgroundButtonShown(true) {}
    QString language;
    bool backgroundButtonShown;
};

ConfigWidget::ConfigWidget(KConfig *, QWidget *parent)
    : QWidget(parent), d(new Private)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QLabel *label = new QLabel(i18n("Spell checking is not available in this build of the editor."), this);
    label->setWordWrap(true);
    layout->addWidget(label);
    layout->addStretch();
}

ConfigWidget::~ConfigWidget()
{
    delete d;
}

bool ConfigWidget::backgroundCheckingButtonShown() const { return d->backgroundButtonShown; }
void ConfigWidget::setLanguage(const QString &language) { d->language = language; }
QString ConfigWidget::language() const { return d->language; }
void ConfigWidget::save() {}
void ConfigWidget::setBackgroundCheckingButtonShown(bool shown) { d->backgroundButtonShown = shown; }
void ConfigWidget::slotDefault() {}

void ConfigWidget::slotChanged()
{
    emit configChanged();
}

ConfigDialog::ConfigDialog(KConfig *config, QWidget *parent)
    : QDialog(parent), m_widget(new ConfigWidget(config, this))
{
    setWindowTitle(i18nc("@title:window", "Spell Checking Configuration"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_widget);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(slotOk()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttons);
    connect(m_widget, SIGNAL(configChanged()), this, SIGNAL(configChanged()));
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::setLanguage(const QString &language) { m_widget->setLanguage(language); }
QString ConfigDialog::language() const { return m_widget->language(); }

void ConfigDialog::slotOk()
{
    slotApply();
    accept();
}

void ConfigDialog::slotApply()
{
    m_widget->save();
    emit configChanged();
}

} // namespace Sonnet
