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


#ifndef _RANDOM_H_
#define _RANDOM_H_

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)

double RANDOM();
int Seed(int seed);
long RandomNumber(long max);
void AskSeed();
int Randomize(void);
int RandomCounter(void);


// Included Random shuffle to retain compatibility with the MacOS standard library
template <class RandomAccessIterator, class RandomNumberGenerator>
  void vl_shuffle (RandomAccessIterator first, RandomAccessIterator last,
                       RandomNumberGenerator& gen) {
  long i, n;
  n = (last-first);
  for (i=n-1; i>0; --i) {
    swap (first[i],first[gen(i+1)]);
  }
}


// Class MyUrand
// to get identical simulations for a given random seed.
class MyUrand {
  long n;
 public:
  typedef long result_type;
  long min() { return 0; }
  long max() { return n; }

  MyUrand(long nn) {
    n=nn;
  }
  MyUrand(void){};

  void seed(long s) {
    Seed(s);
  }

  long operator()(long nn) { return RandomNumber(nn) - 1; }
  long operator()(void) { return RandomNumber(n); }
};

#endif

/* finis */
