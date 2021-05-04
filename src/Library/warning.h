/*
 *
 *  $Id$
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


#ifndef _WARNING_H_
#define _WARNING_H_

#define MEMORYCHECK(x) if ((x)) {   fprintf(stderr, "Out of Memory error in "#x" \n");  exit(0); }

#define UNIDENTIFIED 2353996

#ifdef __cplusplus 
extern "C" { 
#endif
  // namespace MyWarning needed because libxml2 also defines a warning token.

  namespace MyWarning {
    void error(const char *, ...);
    void warning(const char *, ...);
    void unique_warning(const char *, ...);
  }

#ifdef __cplusplus
}
#endif

#endif

/* finis */
