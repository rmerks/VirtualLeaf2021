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

all: tutorial4

tutorial4: Makefile.tutorial4
	$(MAKE) -f Makefile.tutorial4

Makefile.tutorial4: tutorial4.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.tutorial4 clean
ifeq ($(MAKE),make)
	touch tutorial4.pro
else
	copy /b tutorial4.pro +,,
endif

# finis
