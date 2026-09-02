/* qt4compat: kselectaction_p.h - shared with KRecentFilesAction */
#ifndef KDECOMPAT_KSELECTACTION_P_H
#define KDECOMPAT_KSELECTACTION_P_H

#include <kselectaction.h>

#include <QtCore/QList>
#include <QtGui/QComboBox>
#include <QtGui/QToolButton>

class QActionGroup;

class KSelectActionPrivate
{
public:
    KSelectActionPrivate()
        : m_edit(false), m_menuAccelsEnabled(true), m_comboWidth(-1), m_maxComboViewCount(-1),
          m_toolBarMode(KSelectAction::ComboBoxMode), m_toolButtonPopupMode(QToolButton::InstantPopup),
          m_actionGroup(0) {}
    virtual ~KSelectActionPrivate() {}

    QString makeMenuText(const QString &text) const
    {
        if (m_menuAccelsEnabled)
            return text;
        QString t = text;
        t.replace(QLatin1Char('&'), QLatin1String("&&"));
        return t;
    }

    bool m_edit;
    bool m_menuAccelsEnabled;
    int m_comboWidth;
    int m_maxComboViewCount;
    KSelectAction::ToolBarMode m_toolBarMode;
    QToolButton::ToolButtonPopupMode m_toolButtonPopupMode;
    QActionGroup *m_actionGroup;
    QList<QToolButton *> m_buttons;
    QList<QComboBox *> m_comboBoxes;
};

#endif
