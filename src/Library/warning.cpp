/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "warning.h"
#ifdef QTGRAPHICS
#include <qapplication.h>
#include "canvas.h"
#endif
#include <QMessageBox>

extern bool useGUI;
static const std::string _module_id("$Id$");

int Quiet=0;

/*
 * ERROR: scream and die quickly.
 */

#ifndef QTGRAPHICS
void MyWarning::error(char * fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);		/* invoke interface to printf       */
  fprintf(stderr,"\n");     /* automatic \n by Roeland */
  fflush(stderr);			/* drain std error buffer 	    */
  va_end(ap);
  exit(1);				/* quit with error status	    */
}
#else
//#include <qmessagebox.h>
#include "UniqueMessage.h"
void MyWarning::error(const char *fmt, ...)
{
  va_list ap;
  if (Quiet) return;
  char *message = new char[1000];

  va_start(ap, fmt);
  vsnprintf(message, 999, fmt, ap);		/* invoke interface to printf       */
  va_end(ap);

  QString qmess(message);

  if (!useGUI) {
    // batch mode: print the message to stderr
    fprintf(stderr, "Fatal error: %s\n",qmess.toStdString().c_str());
    exit(1);
  } else { // issue a dialog box
    /* Solve this with signal and slot...! */
    //extern MainBase *main_window;
    //((Main *)main_window)->stopSimulation();

    QMessageBox::critical( 0 , "Fatal error", qmess, QMessageBox::Abort, QMessageBox::NoButton, QMessageBox::NoButton );
    fprintf(stderr, "Error: %s\n",qmess.toStdString().c_str());
    QCoreApplication::exit(1);
    std::exit(1);
  }
  delete[] message;
}
#endif


/*
 * EPRINTF: scream, but don't die yet.
 * Roeland changed this to "warning" (21/10/1998)
 * and added an automatic "\n"
 */

#ifndef QTGRAPHICS
void MyWarning::warning(const char * fmt, ...)
{
  va_list ap;
  if (Quiet) return;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);		/* invoke interface to printf       */
  fprintf(stderr,"\n");     /* automatic \n by Roeland */
  fflush(stderr);			/* drain std error buffer 	    */
  va_end(ap);
}
#else 

#include <qmessagebox.h>


void MyWarning::warning(const char *fmt, ...)
{
  va_list ap;
  if (Quiet) return;
  char *message = new char[1000];

  va_start(ap, fmt);
  vsnprintf(message, 999, fmt, ap);		/* invoke interface to printf       */
  va_end(ap);

  QString qmess(message);

  //  bool batch = false;

  if (!useGUI) {
    // batch mode: print the message to stderr
    fprintf(stderr, "Warning: %s\n",qmess.toStdString().c_str());
  } else { // issue a dialog box
    UniqueMessageBox msgBox( QString("Warning"), qmess );
    msgBox.exec();
  }
  delete[] message;
}
#endif

/*! Issues a warning only once,
  by comparing it to a list of warnings issued previously. */
void MyWarning::unique_warning(const char *fmt, ...) {

  va_list ap;
  if (Quiet) return;
  char *message = new char[1000];

  va_start(ap, fmt);
  vsnprintf(message, 999, fmt, ap);		/* invoke interface to printf       */
  va_end(ap);

  static vector<string> previous_warnings;

  // search warning in list
  if (find(
	   previous_warnings.begin(),previous_warnings.end(), 
	   string(message)) 
      ==previous_warnings.end()) {

    // new warning, store in list
    previous_warnings.push_back(string(message));

    // issue warning
    warning("%s", message);

  } else {
    // don't issue warning
    return;
  }
}


#ifdef TESTBED

main(int argc, char * argv[])
{
  eprintf("warning: foo=%f\tbar=%d\tfum=\"%s\"\n", 3.1415, 32768, "waldo");
  error("error: foo=%f\tbar=%d\tfum=\"%s\"\n", 3.1415, 32768, "waldo");
}

#endif

/* finis */
