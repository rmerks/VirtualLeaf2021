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

# get dependencies for executable
sub get_frameworks {
    my $exec = shift;
    my @frameworks = ();
    my $fw;
#  print "get_frameworks says: exec = $exec\n";
    open deps, "otool -L $exec |";
    # omit first line
    <deps>;

    while (<deps>) {
	# look for lines with ".framework", that do not start with / or @.
	if (/\.framework/ && !/^[ \t]*\// && !/^[ \t]*\@/) {
	    chomp;
	    @line = split;
	    $f = $line[0];
	    # get framework directory name
	    @line = split(/\//,$f);
	    for $d (@line) {
		if ($d =~ /\.framework/) {
		    $fw = $d; 
#          print "get_framework finds framework $d\n";
		    last;
		}
	    }
	    if (defined($fw)) {
#	print "get_frameworks pushes $fw on stack\n";
		push @frameworks, $fw;
	    }
	} 
    }
    @frameworks;
}

$app = $ARGV[0];
$appdir = $app.".app";
if ($appdir =~ /\.app\.app/) {
    $appdir = $ARGV[0];
}
$exec = $appdir."/Contents/MacOS/$app";

@frameworks = get_frameworks( $exec );

#for $f (@frameworks) {
#  print "$f\n";
#}

# copy all frameworks to appdir/frameworks

system ("mkdir $appdir/Contents/Frameworks");
for $fw (@frameworks) {
    system "cp -R /Library/Frameworks/$fw $appdir/Contents/Frameworks/.";
}


# set identification names for the frameworks

for $fw (@frameworks) {
    $lib = $fw; $lib =~ s/\.framework//g;
    system "install_name_tool -id \@executable_path/../Frameworks/$fw/Versions/4.0/$lib $appdir/Contents/Frameworks/$fw/Versions/4.0/$lib";

    
    # tell dynamic linker where to look for the frameworks
    system "install_name_tool -change $fw/Versions/4/$lib \@executable_path/../Frameworks/$fw/Versions/4.0/$lib $appdir/Contents/MacOs/$app";
    
}


# now, set the frameworks' reciprocal dependencies right
for $fw (@frameworks) {
    $lib = $fw; $lib =~ s/\.framework//g;
    my @frameworks_of_fw = get_frameworks ( "$appdir/Contents/Frameworks/$fw/Versions/4.0/$lib" );
    for $fwfw (@frameworks_of_fw) {
#    print "$fwfw\n";
	$liblib = $fwfw; $liblib =~ s/\.framework//g;    
	system "install_name_tool -change $fwfw/Versions/4/$liblib \@executable_path/../Frameworks/$fwfw/Versions/4.0/$liblib  $appdir/Contents/Frameworks/$fw/Versions/4.0/$lib";
    }
}

# do the same for additional libs not in a framework depending on frameworks (e.g. libqwt...)
@additionallibs = ( "libqwt.dylib" );

for $lib (@additionallibs) {
    my @frameworks_of_lib = get_frameworks ( "$appdir/Contents/Frameworks/$lib" );
    for $fwfw (@frameworks_of_lib) {
	#    print "$fwfw\n";
	$liblib = $fwfw; $liblib =~ s/\.framework//g;    
	system "install_name_tool -change $fwfw/Versions/4/$liblib \@executable_path/../Frameworks/$fwfw/Versions/4.0/$liblib  $appdir/Contents/Frameworks/$lib";
    }
}

# finis
