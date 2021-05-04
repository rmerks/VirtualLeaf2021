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

#include <QDebug>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include "random.h"

static const std::string _module_id("$Id$");

static int idum = -1;
using namespace std;

static int counter=0;
/*! \return A random double between 0 and 1
**/
double RANDOM(void)
/* Knuth's substrative method, see Numerical Recipes */
{
  static int inext,inextp;
  static long ma[56];
  static int iff=0;
  counter++;
  long mj,mk;
  int i,ii,k;

  if (idum < 0 || iff == 0) {
    iff=1;
    mj=MSEED-(idum < 0 ? -idum : idum);
    mj %= MBIG;
    ma[55]=mj;
    mk=1;
    i=1;
    do {
      ii=(21*i) % 55;
      ma[ii]=mk;
      mk=mj-mk;
      if (mk < MZ) mk += MBIG;
      mj=ma[ii];
    } while ( ++i <= 54 );
    k=1;
    do {
      i=1;
      do {
        ma[i] -= ma[1+(i+30) % 55];
        if (ma[i] < MZ) ma[i] += MBIG;
      } while ( ++i <= 55 );
    } while ( ++k <= 4 );
    inext=0;
    inextp=31;
    idum=1;
  }
  if (++inext == 56) inext=1;
  if (++inextp == 56) inextp=1;
  mj=ma[inext]-ma[inextp];
  if (mj < MZ) mj += MBIG;
  ma[inext]=mj;
  return mj*FAC;
}

/*! \param An integer random seed
  \return the random seed
**/
int Seed(int seed)
{
  if (seed < 0) {
    int rseed=Randomize();
#ifdef QDEBUG
    qDebug() << "Randomizing random generator, seed is " << rseed << endl;
#endif
    return rseed;
  } else {
    int i;
    idum = -seed;
    for (i=0; i <100; i++)
      RANDOM();
    return seed;
  }
}


/*! Returns a random integer value between 1 and 'max'
  \param The maximum value (long)
  \return A random integer (long)
**/
long RandomNumber(long max)
{
  return((long)(RANDOM()*max+1));
}

/*! Interactively ask for the seed
  \param void
  \return void
**/
void AskSeed(void)
{
  int seed;
  int err = 1;
  while (err){
    printf("Please enter a random seed: ");
    err = scanf("%d",&seed);
  }
  printf("\n");
  Seed(seed);
}

int RandomCounter(void) {
  return counter;
}

/*! Make a random seed based on the local time
  \param void
  \return void
**/

int Randomize(void) {

  // Set the seed according to the local time

  int seed;

  auto timepoint = std::chrono::system_clock::now();
  auto since_epoch =  timepoint.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);

  seed=abs((int)((seconds.count()*milliseconds.count())%655337));

  Seed(seed);
#ifdef QDEBUG
  qDebug() << "Random seed is " << seed << endl;
#endif
  return seed;
}

/* finis */
