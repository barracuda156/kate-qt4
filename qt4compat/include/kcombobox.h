/* qt4compat: kcombobox.h - KComboBox with a KLineEdit for editable mode */
#ifndef KDECOMPAT_KCOMBOBOX_H
#define KDECOMPAT_KCOMBOBOX_H

#include <QtGui/QComboBox>
#include <kdemacros.h>
#include <kcompletion.h>
#include <kglobalsettings.h>

class QLineEdit;
class QMenu;
class KCompletionBox;
class KLineEdit;
class KUrl;

class KDEUI_EXPORT KComboBox : public QComboBox, public KCompletionBase
{
    Q_OBJECT
    Q_PROPERTY(bool autoCompletion READ autoCompletion WRITE setAutoCompletion)
    Q_PROPERTY(bool urlDropsEnabled READ urlDropsEnabled WRITE setUrlDropsEnabled)
    Q_PROPERTY(bool trapReturnKey READ trapReturnKey WRITE setTrapReturnKey)
public:
    explicit KComboBox(QWidget *parent = 0);
    explicit KComboBox(bool rw, QWidget *parent = 0);
    virtual ~KComboBox();

    void setEditUrl(const KUrl &url);
    void addUrl(const KUrl &url);
    void addUrl(const QIcon &icon, const KUrl &url);
    void insertUrl(int index, const KUrl &url);
    void insertUrl(int index, const QIcon &icon, const KUrl &url);
    void changeUrl(int index, const KUrl &url);
    void changeUrl(int index, const QIcon &icon, const KUrl &url);
    int cursorPosition() const;
    virtual void setAutoCompletion(bool autocomplete);
    bool autoCompletion() const;
    virtual void setContextMenuEnabled(bool showMenu);
    void setUrlDropsEnabled(bool enable);
    bool urlDropsEnabled() const;
    bool contains(const QString &text) const;
    void setTrapReturnKey(bool trap);
    bool trapReturnKey() const;
    virtual bool eventFilter(QObject *, QEvent *);
    KCompletionBox *completionBox(bool create = true);
    virtual void setLineEdit(QLineEdit *);
    void setEditable(bool editable);

Q_SIGNALS:
    void returnPressed();
    void returnPressed(const QString &);
    void completion(const QString &);
    void substringCompletion(const QString &);
    void textRotation(KCompletionBase::KeyBindingType);
    void completionModeChanged(KGlobalSettings::Completion);
    void aboutToShowContextMenu(QMenu *p);

public Q_SLOTS:
    void rotateText(KCompletionBase::KeyBindingType type);
    virtual void setCompletedText(const QString &);
    void setCompletedItems(const QStringList &items, bool autosubject = true);
    void setCurrentItem(const QString &item, bool insert = false, int index = -1);

protected Q_SLOTS:
    virtual void makeCompletion(const QString &);

protected:
    virtual void setCompletedText(const QString &, bool);
    virtual void wheelEvent(QWheelEvent *ev);
    virtual QSize minimumSizeHint() const;

private Q_SLOTS:
    void lineEditDeleted();

private:
    void init();
    KLineEdit *m_klineEdit;
    bool m_trapReturnKey;
};

#endif
