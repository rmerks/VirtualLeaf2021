#!/usr/bin/perl

#
# $Id$
#
#  This file is part of the Virtual Leaf.
#
#  The Virtual Leaf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  The Virtual Leaf is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
#
#  Copyright 2010 Roeland Merks.
#
#

$n_bins=100;
$min=0.;
$max=8*atan2(1,1); # 2 Pi
$binsize = ($max-$min)/$n_bins;


for ($i=0;$i<$n_bins;$i++) {
    $bins[$i]=0;
}

while (<>) {
    @line=split;
    $num=$line[0];
    $bin = int($num / $binsize);
    # print "[$num, $bin] ";
    $bins[$bin]++;
}


for ($bin=0;$bin<=$#bins;$bin++) {
    $halfwaybin = $bin * $binsize + $binsize/2.;
    print $halfwaybin." ".$bins[$bin]."\n";
}

# finis

