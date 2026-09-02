/* qt4compat: kselectaction.h (kdelibs 4.14 API) */
#ifndef KDECOMPAT_KSELECTACTION_H
#define KDECOMPAT_KSELECTACTION_H

#include <QtGui/QToolButton>
#include <kaction.h>

class KSelectActionPrivate;

class KDEUI_EXPORT KSelectAction : public KAction
{
    Q_OBJECT
    Q_PROPERTY(QAction* currentAction READ currentAction WRITE setCurrentAction)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    Q_PROPERTY(int comboWidth READ comboWidth WRITE setComboWidth)
    Q_PROPERTY(QString currentText READ currentText)
    Q_ENUMS(ToolBarMode)
    Q_PROPERTY(ToolBarMode toolBarMode READ toolBarMode WRITE setToolBarMode)
    Q_PROPERTY(QToolButton::ToolButtonPopupMode toolButtonPopupMode READ toolButtonPopupMode WRITE setToolButtonPopupMode)
    Q_PROPERTY(int currentItem READ currentItem WRITE setCurrentItem)
    Q_PROPERTY(QStringList items READ items WRITE setItems)
public:
    explicit KSelectAction(QObject *parent);
    KSelectAction(const QString &text, QObject *parent);
    KSelectAction(const KIcon &icon, const QString &text, QObject *parent);
    virtual ~KSelectAction();

    enum ToolBarMode {
        MenuMode,
        ComboBoxMode
    };

    ToolBarMode toolBarMode() const;
    void setToolBarMode(ToolBarMode mode);
    QToolButton::ToolButtonPopupMode toolButtonPopupMode() const;
    void setToolButtonPopupMode(QToolButton::ToolButtonPopupMode mode);

    QActionGroup *selectableActionGroup() const;
    QAction *currentAction() const;
    int currentItem() const;
    QString currentText() const;
    QList<QAction *> actions() const;
    QAction *action(int index) const;
    QAction *action(const QString &text, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool setCurrentAction(QAction *action);
    bool setCurrentItem(int index);
    bool setCurrentAction(const QString &text, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    virtual void addAction(QAction *action);
    KAction *addAction(const QString &text);
    KAction *addAction(const KIcon &icon, const QString &text);
    virtual QAction *removeAction(QAction *action);
    void setItems(const QStringList &lst);
    QStringList items() const;
    bool isEditable() const;
    void setEditable(bool);
    int comboWidth() const;
    void setComboWidth(int width);
    void setMaxComboViewCount(int n);
    void clear();
    void removeAllActions();
    void setMenuAccelsEnabled(bool b);
    bool menuAccelsEnabled() const;
    void changeItem(int index, const QString &text);

Q_SIGNALS:
    void triggered(QAction *action);
    void triggered(int index);
    void triggered(const QString &text);

protected Q_SLOTS:
    virtual void actionTriggered(QAction *action);
    void slotToggled(bool);

protected:
    virtual QWidget *createWidget(QWidget *parent);
    virtual void deleteWidget(QWidget *widget);
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);
    KSelectAction(KSelectActionPrivate &dd, QObject *parent);
    KSelectActionPrivate *d_ptr;

private Q_SLOTS:
    void comboBoxActivated(int index);
    void comboBoxDeleted(QObject *object);
    void toolButtonDeleted(QObject *object);
    void syncWidgets();

private:
    void init();
    Q_DECLARE_PRIVATE(KSelectAction)
};

#endif
