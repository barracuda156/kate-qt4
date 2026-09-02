/* qt4compat: ktemporaryfile.cpp */
#include <ktemporaryfile.h>

#include <kcomponentdata.h>
#include <kglobal.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

static QString defaultPrefix()
{
    QString name = QCoreApplication::instance() ? QCoreApplication::applicationName() : QString();
    if (name.isEmpty())
        name = QLatin1String("kate");
    return QDir::tempPath() + QLatin1Char('/') + name;
}

KTemporaryFile::KTemporaryFile()
    : QTemporaryFile()
{
    m_prefix = defaultPrefix();
    updateTemplate();
}

KTemporaryFile::KTemporaryFile(const KComponentData &componentData)
    : QTemporaryFile()
{
    m_prefix = QDir::tempPath() + QLatin1Char('/') + componentData.componentName();
    updateTemplate();
}

KTemporaryFile::~KTemporaryFile()
{
}

void KTemporaryFile::setPrefix(const QString &prefix)
{
    if (QDir::isAbsolutePath(prefix))
        m_prefix = prefix;
    else
        m_prefix = QDir::tempPath() + QLatin1Char('/') + prefix;
    updateTemplate();
}

void KTemporaryFile::setSuffix(const QString &suffix)
{
    m_suffix = suffix;
    updateTemplate();
}

void KTemporaryFile::updateTemplate()
{
    setFileTemplate(m_prefix + QLatin1String("XXXXXX") + m_suffix);
}
