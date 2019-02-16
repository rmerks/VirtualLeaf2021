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

all: tutorial3

tutorial3: Makefile.tutorial3
	$(MAKE) -f Makefile.tutorial3

Makefile.tutorial3: tutorial3.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.tutorial3 clean
ifeq ($(MAKE),make)
	touch tutorial3.pro
else
	copy /b tutorial3.pro +,,
endif

# finis
