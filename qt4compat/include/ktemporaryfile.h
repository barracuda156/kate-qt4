/* qt4compat: ktemporaryfile.h */
#ifndef KDECOMPAT_KTEMPORARYFILE_H
#define KDECOMPAT_KTEMPORARYFILE_H

#include <QtCore/QTemporaryFile>
#include <kdemacros.h>

class KComponentData;

class KDECORE_EXPORT KTemporaryFile : public QTemporaryFile
{
public:
    KTemporaryFile();
    explicit KTemporaryFile(const KComponentData &componentData);
    virtual ~KTemporaryFile();

    void setPrefix(const QString &prefix);
    void setSuffix(const QString &suffix);

private:
    void updateTemplate();
    QString m_prefix;
    QString m_suffix;
};

#endif
