/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
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


#ifndef _FLUX_FUNCTION_h_
#define _FLUX_FUNCTION_h_

// This header file defines a macro "SumFluxFromWalls" that attempts to hide this 
// horrendously confusing member function wrapper construct from VirtualLeaf's end users

// required format of flux_function is:
// double [model class name]::[function name](CellBase *this_cell, CellBase *adjacent_cell, Wall *w)
// e.g.:
// double MyModel::PINflux(CellBase *this_cell, CellBase *adjacent_cell, Wall *w)

#include "far_mem_5.h"

#define SumFluxFromWalls( _vleafcellp_, _flux_function_ ) \
  (( _vleafcellp_->ReduceCellAndWalls<double>( far_3_arg_mem_fun( *this, &_flux_function_ ))))

#endif

/* finis */
