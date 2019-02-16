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

all: tutorial2

tutorial2: Makefile.tutorial2
	$(MAKE) -f Makefile.tutorial2

Makefile.tutorial2: tutorial2.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.tutorial2 clean
ifeq ($(MAKE),make)
	touch tutorial2.pro
else
	copy /b tutorial2.pro +,,
endif

# finis
