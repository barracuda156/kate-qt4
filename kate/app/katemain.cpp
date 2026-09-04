

/* This file is part of the KDE project
   Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

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

#include "kateapp.h"
#include <kate_export.h>
#include <KLocale>
#include <KCmdLineArgs>
#include <KAboutData>
#include <kdebug.h>

#include <QByteArray>

// The D-Bus based "reuse an already running instance" logic that used to
// live here has been removed: this build has no D-Bus, so every invocation
// of Kate simply starts a fresh instance. On macOS, a Finder "open with"
// request for a document reaches the already-running instance through a
// QFileOpenEvent instead, see KateApp::openRequestedUrl().
int main( int argc, char **argv )
{
  // here we go, construct the Kate version
  QByteArray kateVersion = KateApp::kateVersion().toLatin1();

  KAboutData aboutData ("kate", 0, ki18n("Kate"), kateVersion,
                        ki18n( "Kate - Advanced Text Editor" ), KAboutData::License_LGPL_V2,
                        ki18n( "(c) 2000-2013 The Kate Authors" ), KLocalizedString(), "http://www.kate-editor.org");
  aboutData.setOrganizationDomain("kde.org");
  aboutData.addAuthor (ki18n("Christoph Cullmann"), ki18n("Maintainer"), "cullmann@kde.org", "http://www.cullmann.io");
  aboutData.addAuthor (ki18n("Anders Lund"), ki18n("Core Developer"), "anders@alweb.dk", "http://www.alweb.dk");
  aboutData.addAuthor (ki18n("Joseph Wenninger"), ki18n("Core Developer"), "jowenn@kde.org", "http://stud3.tuwien.ac.at/~e9925371");
  aboutData.addAuthor (ki18n("Hamish Rodda"), ki18n("Core Developer"), "rodda@kde.org");
  aboutData.addAuthor (ki18n("Dominik Haumann"), ki18n("Developer & Highlight wizard"), "dhdev@gmx.de");
  aboutData.addAuthor (ki18n("Alexander Neundorf"), ki18n("Developer"), "neundorf@kde.org");
  aboutData.addAuthor (ki18n("Waldo Bastian"), ki18n( "The cool buffersystem" ), "bastian@kde.org" );
  aboutData.addAuthor (ki18n("Charles Samuels"), ki18n("The Editing Commands"), "charles@kde.org");
  aboutData.addAuthor (ki18n("Matt Newell"), ki18n("Testing, ..."), "newellm@proaxis.com");
  aboutData.addAuthor (ki18n("Michael Bartl"), ki18n("Former Core Developer"), "michael.bartl1@chello.at");
  aboutData.addAuthor (ki18n("Michael McCallum"), ki18n("Core Developer"), "gholam@xtra.co.nz");
  aboutData.addAuthor (ki18n("Jochen Wilhemly"), ki18n( "KWrite Author" ), "digisnap@cs.tu-berlin.de" );
  aboutData.addAuthor (ki18n("Michael Koch"), ki18n("KWrite port to KParts"), "koch@kde.org");
  aboutData.addAuthor (ki18n("Christian Gebauer"), KLocalizedString(), "gebauer@kde.org" );
  aboutData.addAuthor (ki18n("Simon Hausmann"), KLocalizedString(), "hausmann@kde.org" );
  aboutData.addAuthor (ki18n("Glen Parker"), ki18n("KWrite Undo History, Kspell integration"), "glenebob@nwlink.com");
  aboutData.addAuthor (ki18n("Scott Manson"), ki18n("KWrite XML Syntax highlighting support"), "sdmanson@alltel.net");
  aboutData.addAuthor (ki18n("John Firebaugh"), ki18n("Patches and more"), "jfirebaugh@kde.org");
  aboutData.addAuthor (ki18n("Pablo Martín"), ki18n("Python Plugin Developer"), "goinnn@gmail.com", "http://github.com/goinnn/");
  aboutData.addAuthor (ki18n("Gerald Senarclens de Grancy"), ki18n("QA and Scripting"), "oss@senarclens.eu", "http://find-santa.eu/");

  aboutData.addCredit (ki18n("Matteo Merli"), ki18n("Highlighting for RPM Spec-Files, Perl, Diff and more"), "merlim@libero.it");
  aboutData.addCredit (ki18n("Rocky Scaletta"), ki18n("Highlighting for VHDL"), "rocky@purdue.edu");
  aboutData.addCredit (ki18n("Yury Lebedev"), ki18n("Highlighting for SQL"));
  aboutData.addCredit (ki18n("Chris Ross"), ki18n("Highlighting for Ferite"));
  aboutData.addCredit (ki18n("Nick Roux"), ki18n("Highlighting for ILERPG"));
  aboutData.addCredit (ki18n("Carsten Niehaus"), ki18n("Highlighting for LaTeX"));
  aboutData.addCredit (ki18n("Per Wigren"), ki18n("Highlighting for Makefiles, Python"));
  aboutData.addCredit (ki18n("Jan Fritz"), ki18n("Highlighting for Python"));
  aboutData.addCredit (ki18n("Daniel Naber"));
  aboutData.addCredit (ki18n("Roland Pabel"), ki18n("Highlighting for Scheme"));
  aboutData.addCredit (ki18n("Cristi Dumitrescu"), ki18n("PHP Keyword/Datatype list"));
  aboutData.addCredit (ki18n("Carsten Pfeiffer"), ki18n("Very nice help"));
  aboutData.addCredit (ki18n("All people who have contributed and I have forgotten to mention"));

  // command line args init and co
  KCmdLineArgs::init (argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("s");
  options.add("start <name>", ki18n("Start Kate with a given session"));
  options.add("startanon", ki18n("Start Kate with a new anonymous session, implies '-n'"));
  options.add("n");
  options.add("new", ki18n("Accepted for compatibility and ignored: this build always starts a new instance"));
  options.add("b");
  options.add("block", ki18n("Accepted for compatibility and ignored (no instance reuse in this build)"));
  options.add("p");
  options.add("pid <pid>", ki18n("Accepted for compatibility and ignored (no instance reuse in this build)"));
  options.add("e");
  options.add("encoding <name>", ki18n("Set encoding for the file to open"));
  options.add("l");
  options.add("line <line>", ki18n("Navigate to this line"));
  options.add("c");
  options.add("column <column>", ki18n("Navigate to this column"));
  options.add("i");
  options.add("stdin", ki18n("Read the contents of stdin"));
  options.add("u");
  options.add("use", ki18n("Accepted for compatibility and ignored (no instance reuse in this build)"));
  options.add("+[URL]", ki18n("Document to open"));
  KCmdLineArgs::addCmdLineOptions (options);
  KCmdLineArgs::addTempFileOption();

  // get our command line args ;)
  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

  // construct the real kate app object ;)
  KateApp app (args);
  if (app.shouldExit()) return 0;

  // execute ourself ;)
  return app.exec();
}

// kate: space-indent on; indent-width 2; replace-tabs on;
