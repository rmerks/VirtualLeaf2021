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

all: tutorial5

tutorial5: Makefile.tutorial5
	$(MAKE) -f Makefile.tutorial5

Makefile.tutorial5: tutorial5.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.tutorial5 clean
ifeq ($(MAKE), make)
	touch tutorial5.pro
else
	copy /b tutorial5.pro +,,
endif

# finis
