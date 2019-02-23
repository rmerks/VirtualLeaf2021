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


#ifndef _SIMITEMBASE_H_
#define _SIMITEMBASE_H_

#include <QGraphicsScene>
#include <QBrush>
#include <assert.h>
class Vector;


template<class TYPE> 
TYPE class_cast(void * symbol)
{
  assert(sizeof(void *) == sizeof(TYPE));
  union
  {
    void * symbol;
    TYPE unknownclass;
  } cast;

  cast.symbol = symbol;
  return cast.unknownclass;
}

class SimItemBase
{
 public:
  SimItemBase( void *v, QGraphicsScene *canvas );
  virtual ~SimItemBase(void);
  virtual void userMove(double dx, double dy);


 protected:

  // I know which simulation object I represent, so if I am moved around
  // the canvas, the real object can be moved as well

  // (both Cell and Node have Vector as base class...)
  // Not proper design... sorry.

  void *obj; 
};

#endif

/* finis */
