/* qt4compat: kurlrequester.h - trimmed local replacement for kdelibs'
 * KUrlRequester/KUrlComboRequester. Upstream sits on KIO (remote directory
 * listings via KUrlComboBox) and completes through KUrlCompletion; Kate on
 * plain Qt 4 only ever browses the local filesystem, so this is a line
 * edit (or, for KUrlComboRequester, a combo box) plus a "..." button that
 * opens KFileDialog (kfiledialog.h, local files only already). Public API
 * names (setUrl/url, setMode/mode with the KFile::Mode flags, comboBox(),
 * lineEdit(), button(), returnPressed()) match kdelibs so callers written
 * against it - and .ui files promoting a widget to these classes, e.g.
 * addons/kate/search/search.ui - compile unmodified. */
#ifndef KDECOMPAT_KURLREQUESTER_H
#define KDECOMPAT_KURLREQUESTER_H

#include <QtGui/QFrame>
#include <kdeui_export.h>
#include <kurl.h>
#include <kfile.h>

class KLineEdit;
class KComboBox;
class KPushButton;

class KDEUI_EXPORT KUrlRequester : public QFrame
{
    Q_OBJECT
public:
    explicit KUrlRequester(QWidget *parent = 0);
    explicit KUrlRequester(const KUrl &url, QWidget *parent = 0);
    virtual ~KUrlRequester();

    virtual void setUrl(const KUrl &url);
    KUrl url() const;

    /** convenience used throughout Kate: the requester's current text, unparsed */
    void setText(const QString &text);
    QString text() const;

    void setStartDir(const KUrl &startDir);

    KLineEdit *lineEdit() const;
    /** the edit widget as a KComboBox, or 0 unless this is a KUrlComboRequester */
    KComboBox *comboBox() const;
    KPushButton *button() const;

    KFile::Modes mode() const;
    virtual void setMode(KFile::Modes mode);

    QString filter() const;
    virtual void setFilter(const QString &filter);

Q_SIGNALS:
    void textChanged(const QString &text);
    void urlSelected(const KUrl &url);
    void returnPressed();
    void returnPressed(const QString &text);

protected:
    /** used by KUrlComboRequester to get a combo-box editor instead of a plain line edit */
    KUrlRequester(bool useComboBox, QWidget *parent);

private Q_SLOTS:
    void slotOpenDialog();

private:
    void init(bool useComboBox);

    QWidget *m_edit; // KLineEdit* or KComboBox*, owns itself (child of this)
    KPushButton *m_button;
    KFile::Modes m_mode;
    QString m_filter;
    KUrl m_startDir;
};

/** KUrlRequester whose edit widget is a KComboBox, so comboBox() is never 0 */
class KDEUI_EXPORT KUrlComboRequester : public KUrlRequester
{
    Q_OBJECT
public:
    explicit KUrlComboRequester(QWidget *parent = 0);
};

#endif
