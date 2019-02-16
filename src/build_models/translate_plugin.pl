#!/usr/bin/perl

$cfilename = shift(@ARGV) || die "Usage: translate_plugin.pl [cfile] [hfile] [profile]\n";
$hfilename = shift(@ARGV) || die "Usage: translate_plugin.pl [cfile] [hfile] [profile]\n";
$pfilename = shift(@ARGV) || die "Usage: translate_plugin.pl [cfile] [hfile] [profile]\n";

$ocfname = $cfilename; $ocfname =~ s/\.cpp/_tl.cpp/g;
$ohfname = $hfilename; $ohfname =~ s/\.h/_tl.h/g;
$opfname = $pfilename; $opfname =~ s/\.pro/_tl.pro/g;

print STDERR "Translating '$cfilename' to '$ocfname', '$hfilename' to '$ohfname', and '$pfilename' to '$opfname'\n";

open cfile,"<$cfilename";
open ocfile,">$ocfname";

while (<cfile>) {
    
    #s/$hfilename/$ohfname/g;
    
    # translate function definitions
    if (/[a-zA-Z0-9 ]*::OnDivide/) {
	s/ParentInfo &parent_info/ParentInfo *parent_info/g;
	s/CellBase &daughter1/CellBase *daughter1/g;
	s/CellBase &daughter2/CellBase *daughter2/g;
    }
    
    if (/[a-zA-Z0-9 ]*::SetCellColor/) {
	s/CellBase &c/CellBase *c/g;
	s/QColor &color/QColor *color/g;
    }
    
    if (/[a-zA-Z0-9 ]*::CellHouseKeeping/) {
	s/CellBase &c/CellBase *c/g;
    }
    
    # translate member function calls
    s/\bparent_info\b\./parent_info->/g;
    s/\bdaughter1\b\./daughter1->/g;
    s/\bdaughter2\b\./daughter2->/g;
    s/\bc\b\./c->/g;
    s/\bcolor\b\./color->/g;
    print ocfile;
    
}

open hfile,"<$hfilename";
open ohfile,">$ohfname";

while (<hfile>) {
    if (/[ \t]*virtual[ \t]+void[ \t]+CellHouseKeeping/) {
	s/CellBase &c/CellBase *c/g;
    }
    if (/[ \t]*virtual[ \t]+void[ \t]+OnDivide/) {
	s/ParentInfo &parent_info/ParentInfo *parent_info/g;
	s/CellBase &daughter1/CellBase *daughter1/g;
	s/CellBase &daughter2/CellBase *daughter2/g;
    }
    if (/[ \t]*virtual[ \t]+void[ \t]+SetCellColor/) {
	s/CellBase &c/CellBase *c/g;
	s/QColor &color/QColor *color/g;
    }
    
    
    print ohfile;
    
}

open pfile,"<$pfilename";
open opfile,">$opfname";

while (<pfile>) {
    
    s/\bplugin\b\.h/plugin_tl\.h/g;
    s/\bplugin\b\.cpp/plugin_tl\.cpp/g;
    
    print opfile;
    
}

#finis
