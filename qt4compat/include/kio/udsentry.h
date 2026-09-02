/* qt4compat: kio/udsentry.h - minimal UDSEntry so local-only code compiles */
#ifndef KDECOMPAT_KIO_UDSENTRY_H
#define KDECOMPAT_KIO_UDSENTRY_H

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QString>
#include <kdemacros.h>

namespace KIO
{
    class KIO_EXPORT UDSEntry
    {
    public:
        enum StandardFieldTypes {
            UDS_STRING = 0x01000000,
            UDS_NUMBER = 0x02000000,
            UDS_TIME = 0x04000000 | UDS_NUMBER,
            UDS_SIZE = 1 | UDS_NUMBER,
            UDS_USER = 2 | UDS_STRING,
            UDS_ICON_NAME = 3 | UDS_STRING,
            UDS_GROUP = 4 | UDS_STRING,
            UDS_NAME = 5 | UDS_STRING,
            UDS_LOCAL_PATH = 6 | UDS_STRING,
            UDS_HIDDEN = 7 | UDS_NUMBER,
            UDS_ACCESS = 8 | UDS_NUMBER,
            UDS_MODIFICATION_TIME = 9 | UDS_TIME,
            UDS_ACCESS_TIME = 10 | UDS_TIME,
            UDS_CREATION_TIME = 11 | UDS_TIME,
            UDS_FILE_TYPE = 12 | UDS_NUMBER,
            UDS_LINK_DEST = 13 | UDS_STRING,
            UDS_URL = 14 | UDS_STRING,
            UDS_MIME_TYPE = 15 | UDS_STRING,
            UDS_GUESSED_MIME_TYPE = 16 | UDS_STRING,
            UDS_XML_PROPERTIES = 17 | UDS_STRING,
            UDS_EXTENDED_ACL = 18 | UDS_NUMBER,
            UDS_ACL_STRING = 19 | UDS_STRING,
            UDS_DEFAULT_ACL_STRING = 20 | UDS_STRING,
            UDS_DISPLAY_NAME = 21 | UDS_STRING,
            UDS_TARGET_URL = 22 | UDS_STRING,
            UDS_DISPLAY_TYPE = 23 | UDS_STRING,
            UDS_EXTRA = 100 | UDS_STRING,
            UDS_EXTRA_END = 140 | UDS_STRING
        };

        UDSEntry() {}
        QString stringValue(uint field) const { return m_strings.value(field); }
        long long numberValue(uint field, long long defaultValue = 0) const { return m_numbers.contains(field) ? m_numbers.value(field) : defaultValue; }
        bool isDir() const;
        bool isLink() const { return !stringValue(UDS_LINK_DEST).isEmpty(); }
        void insert(uint field, const QString &value) { m_strings.insert(field, value); }
        void insert(uint field, long long value) { m_numbers.insert(field, value); }
        int count() const { return m_strings.count() + m_numbers.count(); }
        bool contains(uint field) const { return m_strings.contains(field) || m_numbers.contains(field); }
        void clear() { m_strings.clear(); m_numbers.clear(); }
        QList<uint> listFields() const { return m_strings.keys() + m_numbers.keys(); }

    private:
        QHash<uint, QString> m_strings;
        QHash<uint, long long> m_numbers;
    };

    typedef QList<UDSEntry> UDSEntryList;
}

#include <sys/stat.h>
inline bool KIO::UDSEntry::isDir() const
{
    return S_ISDIR(numberValue(UDS_FILE_TYPE));
}

#endif
