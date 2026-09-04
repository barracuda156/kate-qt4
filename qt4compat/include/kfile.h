/* qt4compat: kfile.h - the KFile::Mode flags kdelibs' KFileDialog/KUrlRequester
 * use to select what a file picker offers (file vs. directory, local-only,
 * must-exist, ...). Kate's static KFileDialog (kfiledialog.h) does not need
 * these itself, but KUrlRequester::setMode() and several .ui files (promoted
 * widgets set the "mode" property, e.g. addons/kate/search/search.ui) are
 * written against kdelibs and expect the KFile namespace to exist. */
#ifndef KDECOMPAT_KFILE_H
#define KDECOMPAT_KFILE_H

#include <QtCore/QFlags>

namespace KFile
{
    enum Mode {
        File            = 1,
        Directory       = 2,
        Files           = 4,
        ExistingOnly    = 8,
        LocalOnly       = 16,
        Default         = File
    };
    Q_DECLARE_FLAGS(Modes, Mode)

    enum FileView {
        Default_View  = 0,
        Simple        = 1,
        Detail        = 2,
        SeparateDirs  = 4,
        PreviewContents = 8,
        PreviewInfo   = 16
    };
    Q_DECLARE_FLAGS(FileViews, FileView)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KFile::Modes)
Q_DECLARE_OPERATORS_FOR_FLAGS(KFile::FileViews)

#endif
