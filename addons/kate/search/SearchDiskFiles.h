/*   Kate search plugin
 * 
 * Copyright (C) 2011-2013 by Kåre Särs <kare.sars@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file called COPYING; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef SearchDiskFiles_h
#define SearchDiskFiles_h

#include <QThread>
#include <QFileInfo>
#include <QVector>
#include <QMutex>
#include <QStringList>
#include <QTime>

class KateRegExp;

class SearchDiskFiles: public QThread
{
    Q_OBJECT

public:
    SearchDiskFiles(QObject *parent = 0);
    ~SearchDiskFiles();

    void startSearch(const QStringList &iles,
                     const KateRegExp &regexp);
    void run();

    bool searching();

private:
    void searchSingleLineRegExp(const QString &fileName);
    void searchMultiLineRegExp(const QString &fileName);

public Q_SLOTS:
    void cancelSearch();

Q_SIGNALS:
    void matchFound(const QString &url, const QString &docName, int line, int column,
                    const QString &lineContent, int matchLen);
    void searchDone();
    void searching(const QString &file);

private:
    // owned copy for this search run, so the worker thread never touches the
    // GUI thread's KateRegExp (see kateregexp.h: copies have independent
    // match state, sharing only the read-only compiled pattern)
    KateRegExp      *m_regExp;
    QStringList      m_files;
    bool             m_cancelSearch;
    int              m_matchCount;
    QTime            m_statusTime;
};


#endif
