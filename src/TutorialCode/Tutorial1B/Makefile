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

all: tutorial1B

tutorial1B: Makefile.tutorial1B
	$(MAKE) -f Makefile.tutorial1B

Makefile.tutorial1B: tutorial1B.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.tutorial1B clean
ifeq ($(MAKE),make)
	touch tutorial1B.pro
else
	copy /b tutorial1B.pro +,,
endif

# finis
