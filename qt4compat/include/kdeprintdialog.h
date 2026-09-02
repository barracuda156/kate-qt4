/* qt4compat: kdeprintdialog.h - plain QPrintDialog */
#ifndef KDECOMPAT_KDEPRINTDIALOG_H
#define KDECOMPAT_KDEPRINTDIALOG_H

#include <QtCore/QList>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QWidget>
#include <kdemacros.h>

namespace KdePrint
{
    enum SettingsFlag {
        DefaultSettings = 0x00,
        SystemSelectsPages = 0x01
    };
    Q_DECLARE_FLAGS(SettingsFlags, SettingsFlag)

    inline QPrintDialog *createPrintDialog(QPrinter *printer, const QList<QWidget *> &customTabs, QWidget *parent = 0)
    {
        QPrintDialog *dialog = new QPrintDialog(printer, parent);
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
        dialog->setOptionTabs(customTabs);
#else
        Q_UNUSED(customTabs)
#endif
        return dialog;
    }

    inline QPrintDialog *createPrintDialog(QPrinter *printer, QWidget *parent = 0)
    {
        return new QPrintDialog(printer, parent);
    }

    inline QPrintDialog *createPrintDialog(QPrinter *printer, SettingsFlags flags, const QList<QWidget *> &customTabs, QWidget *parent = 0)
    {
        QPrintDialog *dialog = createPrintDialog(printer, customTabs, parent);
        if (flags & SystemSelectsPages)
            dialog->setOption(QAbstractPrintDialog::PrintPageRange, true);
        return dialog;
    }

    inline QPrintDialog *createPrintDialog(QPrinter *printer, SettingsFlags flags, QWidget *parent = 0)
    {
        return createPrintDialog(printer, flags, QList<QWidget *>(), parent);
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KdePrint::SettingsFlags)

#endif
