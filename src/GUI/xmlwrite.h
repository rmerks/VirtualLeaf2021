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


#ifndef _XML_WRITE_H_
#define _XML_WRITE_H_

#include <vector>
#include <libxml/tree.h>
#include <cstring>

namespace XMLIO {

  std::vector <double> XMLReadValArray(xmlNode *cur);
  template<class For, class E> long list_index (For beg, For end, E *elem) {

    long n = 0;
    for (For i=beg;
	 i!=end;
	 i++) {
      if (*i == elem) {
	return n;
      }
      n++;
    }
    return -1;
  }
}


static const char * bool_names[2] = {"true","false"};

inline const char *bool_name(bool q) { return q ? bool_names[0] : bool_names[1]; }

inline bool strtobool(const char *str) { 
  if (!strcmp(str, "true")) {
    return true;
  } else {
    if (!strcmp(str, "false")) {
      return false;
    } else {
      throw("Error in xmlwrite.cpp : strtobool(const char *str). Parameter passed other than \"true\" or \"false\".");
    }
  }
}

#endif

/* finis */
