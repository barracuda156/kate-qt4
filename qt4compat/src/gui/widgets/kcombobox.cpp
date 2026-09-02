/* qt4compat: kcombobox.cpp */
#include <kcombobox.h>

#include <kcompletionbox.h>
#include <klineedit.h>
#include <kurl.h>

#include <QtGui/QApplication>
#include <QtGui/QWheelEvent>

KComboBox::KComboBox(QWidget *parent)
    : QComboBox(parent), KCompletionBase(), m_klineEdit(0), m_trapReturnKey(false)
{
    init();
}

KComboBox::KComboBox(bool rw, QWidget *parent)
    : QComboBox(parent), KCompletionBase(), m_klineEdit(0), m_trapReturnKey(false)
{
    init();
    setEditable(rw);
}

KComboBox::~KComboBox()
{
}

void KComboBox::init()
{
    setCompletionMode(KGlobalSettings::completionMode());
    setInsertPolicy(NoInsert);
    connect(this, SIGNAL(activated(QString)), this, SLOT(setCompletedText(QString)));
}

void KComboBox::setEditUrl(const KUrl &url)
{
    if (!lineEdit())
        setEditable(true);
    lineEdit()->setText(url.pathOrUrl());
}

void KComboBox::addUrl(const KUrl &url)
{
    addItem(url.pathOrUrl());
}

void KComboBox::addUrl(const QIcon &icon, const KUrl &url)
{
    addItem(icon, url.pathOrUrl());
}

void KComboBox::insertUrl(int index, const KUrl &url)
{
    insertItem(index, url.pathOrUrl());
}

void KComboBox::insertUrl(int index, const QIcon &icon, const KUrl &url)
{
    insertItem(index, icon, url.pathOrUrl());
}

void KComboBox::changeUrl(int index, const KUrl &url)
{
    setItemText(index, url.pathOrUrl());
}

void KComboBox::changeUrl(int index, const QIcon &icon, const KUrl &url)
{
    setItemIcon(index, icon);
    setItemText(index, url.pathOrUrl());
}

int KComboBox::cursorPosition() const
{
    return lineEdit() ? lineEdit()->cursorPosition() : -1;
}

void KComboBox::setAutoCompletion(bool autocomplete)
{
    if (m_klineEdit) {
        if (autocomplete) {
            m_klineEdit->setCompletionMode(KGlobalSettings::CompletionAuto);
            setCompletionMode(KGlobalSettings::CompletionAuto);
        } else {
            m_klineEdit->setCompletionMode(KGlobalSettings::completionMode());
            setCompletionMode(KGlobalSettings::completionMode());
        }
    }
}

bool KComboBox::autoCompletion() const
{
    return completionMode() == KGlobalSettings::CompletionAuto;
}

void KComboBox::setContextMenuEnabled(bool showMenu)
{
    if (m_klineEdit)
        m_klineEdit->setContextMenuEnabled(showMenu);
}

void KComboBox::setUrlDropsEnabled(bool enable)
{
    if (m_klineEdit)
        m_klineEdit->setUrlDropsEnabled(enable);
}

bool KComboBox::urlDropsEnabled() const
{
    return m_klineEdit && m_klineEdit->urlDropsEnabled();
}

bool KComboBox::contains(const QString &text) const
{
    if (text.isEmpty())
        return false;
    const int itemCount = count();
    for (int i = 0; i < itemCount; ++i) {
        if (itemText(i) == text)
            return true;
    }
    return false;
}

void KComboBox::setTrapReturnKey(bool trap)
{
    m_trapReturnKey = trap;
    if (m_klineEdit)
        m_klineEdit->setTrapReturnKey(trap);
}

bool KComboBox::trapReturnKey() const
{
    return m_trapReturnKey;
}

bool KComboBox::eventFilter(QObject *o, QEvent *ev)
{
    return QComboBox::eventFilter(o, ev);
}

KCompletionBox *KComboBox::completionBox(bool create)
{
    if (m_klineEdit)
        return m_klineEdit->completionBox(create);
    return 0;
}

void KComboBox::setLineEdit(QLineEdit *edit)
{
    if (!isEditable() && edit && !qobject_cast<KLineEdit *>(edit)) {
        QComboBox::setLineEdit(edit);
        return;
    }
    if (!edit)
        edit = new KLineEdit(this);
    QComboBox::setLineEdit(edit);
    m_klineEdit = qobject_cast<KLineEdit *>(edit);
    setDelegate(m_klineEdit);
    if (m_klineEdit) {
        connect(m_klineEdit, SIGNAL(destroyed()), this, SLOT(lineEditDeleted()));
        connect(m_klineEdit, SIGNAL(returnPressed(QString)), this, SIGNAL(returnPressed(QString)));
        connect(m_klineEdit, SIGNAL(completion(QString)), this, SIGNAL(completion(QString)));
        connect(m_klineEdit, SIGNAL(substringCompletion(QString)), this, SIGNAL(substringCompletion(QString)));
        connect(m_klineEdit, SIGNAL(textRotation(KCompletionBase::KeyBindingType)), this, SIGNAL(textRotation(KCompletionBase::KeyBindingType)));
        connect(m_klineEdit, SIGNAL(completionModeChanged(KGlobalSettings::Completion)), this, SIGNAL(completionModeChanged(KGlobalSettings::Completion)));
        connect(m_klineEdit, SIGNAL(aboutToShowContextMenu(QMenu*)), this, SIGNAL(aboutToShowContextMenu(QMenu*)));
        connect(m_klineEdit, SIGNAL(completionBoxActivated(QString)), this, SIGNAL(activated(QString)));
        m_klineEdit->setTrapReturnKey(m_trapReturnKey);
    }
    connect(edit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
}

void KComboBox::setEditable(bool editable)
{
    if (editable) {
        KLineEdit *edit = new KLineEdit(this);
        edit->setClearButtonShown(true);
        setLineEdit(edit);
    } else {
        QComboBox::setEditable(editable);
    }
}

void KComboBox::rotateText(KCompletionBase::KeyBindingType type)
{
    if (m_klineEdit)
        m_klineEdit->rotateText(type);
}

void KComboBox::setCompletedText(const QString &text, bool marked)
{
    if (m_klineEdit)
        m_klineEdit->setCompletedText(text, marked);
}

void KComboBox::setCompletedText(const QString &text)
{
    if (m_klineEdit)
        m_klineEdit->setCompletedText(text);
}

void KComboBox::setCompletedItems(const QStringList &items, bool autosubject)
{
    if (m_klineEdit)
        m_klineEdit->setCompletedItems(items, autosubject);
}

void KComboBox::setCurrentItem(const QString &item, bool insert, int index)
{
    int sel = -1;
    const int itemCount = count();
    for (int i = 0; i < itemCount; ++i) {
        if (itemText(i) == item) {
            sel = i;
            break;
        }
    }
    if (sel == -1 && insert) {
        if (index >= 0) {
            insertItem(index, item);
            sel = index;
        } else {
            addItem(item);
            sel = count() - 1;
        }
    }
    setCurrentIndex(sel);
}

void KComboBox::makeCompletion(const QString &text)
{
    if (m_klineEdit) {
        m_klineEdit->makeCompletion(text);
        return;
    }
    if (text.isNull() || !isVisible())
        return;
    const int index = findText(text, Qt::MatchStartsWith);
    if (index >= 0)
        setCurrentIndex(index);
}

void KComboBox::wheelEvent(QWheelEvent *ev)
{
    QComboBox::wheelEvent(ev);
}

QSize KComboBox::minimumSizeHint() const
{
    QSize size = QComboBox::minimumSizeHint();
    if (isEditable() && m_klineEdit && m_klineEdit->isClearButtonShown()) {
        const int frame = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
        size.setWidth(size.width() + m_klineEdit->clearButtonUsedSize().width() + frame);
    }
    return size;
}

void KComboBox::lineEditDeleted()
{
    if (delegate() == m_klineEdit)
        setDelegate(0);
    m_klineEdit = 0;
}
