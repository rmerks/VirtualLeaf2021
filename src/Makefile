# $Id$

# Set an environment varible $(MAKE) to the name of the make command
# on your machine, e.g. 'make' on UNIX and 'mingw32-make' on windows.
# Fallback default is UNIX make.

ifeq ($(MAKE),)
	MAKE=make
endif

# Ditto for QMAKE
ifeq ($(QMAKE),)
	QMAKE=qmake
endif

all: VirtualLeaf libplugin plugins tutorials

VirtualLeaf: Makefile.VirtualLeaf
	$(MAKE) -f Makefile.VirtualLeaf

Makefile.VirtualLeaf: VirtualLeaf.pro
	$(QMAKE) -o $@ $< 

libplugin: Makefile.libplugin
	$(MAKE) -f Makefile.libplugin

Makefile.libplugin: libplugin.pro
	$(QMAKE) -o $@ $< 

plugins:
	$(MAKE) -C build_models -f Makefile

tutorials:
	$(MAKE) -C build_models -f Makefile
	$(MAKE) -C TutorialCode -f Makefile

perl: Makefile.perl
	$(MAKE) -f Makefile.perl

clean:
	$(MAKE) -f Makefile.libplugin clean
	$(MAKE) -f Makefile.VirtualLeaf clean
	$(MAKE) -C build_models -f Makefile clean
	$(MAKE) -C TutorialCode -f Makefile clean
ifeq ($(MAKE),make)
	touch VirtualLeaf.pro
	touch libplugin.pro
else
	copy /b VirtualLeaf.pro +,,
	copy /b libplugin.pro +,,
endif

#finis
