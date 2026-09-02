/* qt4compat: kde_file.h - large-file aware POSIX wrappers (Unix only) */
#ifndef KDECOMPAT_KDE_FILE_H
#define KDECOMPAT_KDE_FILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <utime.h>

#include <QtCore/QFile>
#include <kdemacros.h>

#define KDE_stat        ::stat
#define KDE_lstat       ::lstat
#define KDE_fstat       ::fstat
#define KDE_open        ::open
#define KDE_lseek       ::lseek
#define KDE_fseek       ::fseeko
#define KDE_ftell       ::ftello
#define KDE_fopen       ::fopen
#define KDE_rename      ::rename
#define KDE_mkdir       ::mkdir
#define KDE_readdir     ::readdir
#define KDE_sendfile    ::sendfile
#define KDE_struct_stat struct stat
#define KDE_struct_dirent struct dirent
#define KDE_off_t       off_t
#define KDE_fseek       ::fseeko
#define KDE_signal      ::signal

namespace KDE
{
    inline int access(const QString &path, int mode) { return ::access(QFile::encodeName(path).constData(), mode); }
    inline int chmod(const QString &path, mode_t mode) { return ::chmod(QFile::encodeName(path).constData(), mode); }
    inline int open(const QString &pathname, int flags, mode_t mode = 0) { return ::open(QFile::encodeName(pathname).constData(), flags, mode); }
    inline FILE *fopen(const QString &pathname, const char *mode) { return ::fopen(QFile::encodeName(pathname).constData(), mode); }
    inline int lstat(const QString &path, KDE_struct_stat *buf) { return ::lstat(QFile::encodeName(path).constData(), buf); }
    inline int mkdir(const QString &pathname, mode_t mode) { return ::mkdir(QFile::encodeName(pathname).constData(), mode); }
    inline int stat(const QString &path, KDE_struct_stat *buf) { return ::stat(QFile::encodeName(path).constData(), buf); }
    inline int rename(const QString &in, const QString &out) { return ::rename(QFile::encodeName(in).constData(), QFile::encodeName(out).constData()); }
    inline int utime(const QString &filename, struct utimbuf *buf) { return ::utime(QFile::encodeName(filename).constData(), buf); }
}

#endif
