/* qt4compat: vendored from kdelibs 4.14 kdeui/widgets/kvbox.cpp (moc includes moved to the end of the file, export macros mapped to kdemacros.h; further changes noted inline) */
/* This file is part of the KDE libraries
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kvbox.h"

KVBox::KVBox( QWidget* parent )
  : KHBox( true, parent ),
    d( 0 )
{
}

KVBox::~KVBox()
{
}


/* qt4compat: moc output is generated into the build dir by qt4_generate_moc (see sources-widgets.cmake) */
#include "kvbox.moc"
