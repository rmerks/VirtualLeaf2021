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

# Store essential model source code in XML files, so the exact model
# used for the geometry can be retrieved later on The crucial model
# description is in the files "reactions.h" (or similar) and leaf.cpp
# (or similar).

# First we should retrieve the name of the main code (i.e. leaf.cpp or
# so), and the version of reactions.h that we include there.

# We can get this from the project file (possibly called "LeafGrowth.pro")

# If more than one project file found, issue an error

open prlist, "ls *.pro |";

@projectfiles = ();
while (<prlist>) {
    chomp;
    push @projectfiles, $_;
}

if ($#projectfiles > 0) {
    print stderr "Oops, I found several project files: ";
    for $p (@projectfiles) {
	print stderr "$p ";
    }
    print stderr "\n";
    print stderr "Please make sure you have only one, and try again\n";
    exit(1);
}

if ($#projectfiles < 0) {
    print stderr "No project files found. Please add a project file.\n";
    exit(1);
}

$project = $projectfiles[0];

print stderr "Using project file \"$project\"\n";

# extract TARGET and REACTIONS from project file

open pfile,"<$project";
my @targets = ();
while (<pfile>) {
    if (/^[ \t]*TARGET/) {
	my @line = split(/=/);
	my $target = $line[1];
	chomp $target;
	$target =~ s/[ \t]//g;
	push @targets, $target;
    }
    if (/^[ \t]*REACTIONS/) {
	my @line = split(/=/);
	my $reaction = $line[1];
	chomp $reaction;
	$reaction =~ s/[ \t]//g;
	push @reactions, $reaction;
    }
}


if ($#targets > 0) {
    print stderr "Too many targets found: ";
    for $t (@targets) {
	print stderr "$t ";
    }
    print stderr "\n";
    exit(1);
}

if ($#targets < 0) {
    print stderr "No targets found in project $project\n";
    exit(1);
}

$target = $targets[0];

print "Target is $target\n";

if ($#reactions > 0) {
    print stderr "Too many reactions found: ";
    for $t (@reactions) {
	print stderr "$t ";
    }
    print stderr "\n";
    exit(1);
}

if ($#reactions < 0) {
    print stderr "No reactions found in project $project\n";
    exit(1);
}

$reaction = $reactions[0];

print "Reactions header is $reaction\n";

$mainsource = $target.".cpp";

# extract "reactions" source from mainsource
# assuming that the header file name starts with "reactions_"

#open sfile, "<$mainsource";
#
#while (<sfile>) {
#  if (/^[ \t]*\#include/) {
#    chomp;
#    my @line = split(/\#include/);
#    my $header = $line[1];
#    $header =~ s/[\t ]//g;
#    if ($header =~ /reactions/) {
#      $header =~ s/[\"\<\>]//g;
#      push @headers, $header;
#    }
#  }
#}
#
#if ($#headers > 0) {
#  print stderr "Oops, more than one reactions header found: ";
#  for $h (@headers) {
#    print stderr "$h ";
#  }
#  print stderr "\n";
#  print stderr "Don't know what to do now... Please rename a header in $mainsource, so only one will start with \"reactions\"\n";
#  exit(1);
#}
#
#if ($#headers < 0) {
#  print stderr "Oops, no reactions header found in $mainsource.\n";
#  print stderr "Please make sure the name of your reactions header starts with \"reactions\"\n";
#  exit(1);
#}
#
#$reactions = $headers[0];



# now generating xmlcode to write the contents of these files (hard coded in the executables)

open xmlsrc, ">xmlwritecode.cpp";

print xmlsrc <<END_MARKER;
// Automatically produced by perl script \"make_xmlwritecode.pl\". DO NOT EDIT

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include "xmlwrite.h"


void XMLIO::XMLWriteLeafSourceCode(xmlNode *parent) {

// Embed the code in our xml file, so we can reconstruct the model we used to produce it ... 

END_MARKER

sub construct_string_constant_from_file {

    my $fname = shift;
    my $str = "";
		
    open srcfile, "<$fname";
    while (<srcfile>) {
	chomp;      
	s/\\/\\\\/g;
	s/\"/\\\"/g;
	s/\&/\&amp;/g;
	$str.=$_."\\n";
    }
    $str .= "\";\n";
    return $str;
}

print xmlsrc "xmlChar *sourcecode = (xmlChar *)\"".construct_string_constant_from_file( "$mainsource" );
print xmlsrc <<END_MARKER2;
xmlNodePtr xmlcode = xmlNewChild(parent, NULL, BAD_CAST \"code\", sourcecode);

{
    xmlNewProp(xmlcode, BAD_CAST "name", BAD_CAST \"$mainsource\");
}

}

void XMLIO::XMLWriteReactionsCode(xmlNode *parent) {

END_MARKER2

print xmlsrc "xmlChar *sourcecode = (xmlChar *)\"".construct_string_constant_from_file( "$reaction" );
print xmlsrc <<END_MARKER3;
xmlNodePtr xmlcode = xmlNewChild(parent, NULL, BAD_CAST \"code\", sourcecode);

{
    xmlNewProp(xmlcode, BAD_CAST "name", BAD_CAST \"$reaction\");
}

}

END_MARKER3

# finis
