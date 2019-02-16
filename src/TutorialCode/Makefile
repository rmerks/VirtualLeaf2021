# $Id$

# Set an environment varible $(MAKE) to the name of the make command
# on your machine, e.g. 'make' on UNIX and 'mingw32-make' on windows.
# Fallback default is UNIX make.

ifeq ($(MAKE),)
	MAKE=make
endif

all: tutorial_0 tutorial_1A tutorial_1B tutorial_1C tutorial_1D tutorial_2 tutorial_3 tutorial_4 tutorial_5

tutorial_0:
	$(MAKE) -C Tutorial0 -f Makefile

tutorial_1A:
	$(MAKE) -C Tutorial1A -f Makefile

tutorial_1B:
	$(MAKE) -C Tutorial1B -f Makefile

tutorial_1C:
	$(MAKE) -C Tutorial1C -f Makefile

tutorial_1D:
	$(MAKE) -C Tutorial1D -f Makefile

tutorial_2:
	$(MAKE) -C Tutorial2 -f Makefile

tutorial_3:
	$(MAKE) -C Tutorial3 -f Makefile

tutorial_4:
	$(MAKE) -C Tutorial4 -f Makefile

tutorial_5:
	$(MAKE) -C Tutorial5 -f Makefile

clean:
	$(MAKE) -C Tutorial0  -f Makefile clean
	$(MAKE) -C Tutorial1A -f Makefile clean
	$(MAKE) -C Tutorial1B -f Makefile clean
	$(MAKE) -C Tutorial1C -f Makefile clean
	$(MAKE) -C Tutorial1D -f Makefile clean
	$(MAKE) -C Tutorial2  -f Makefile clean
	$(MAKE) -C Tutorial3  -f Makefile clean
	$(MAKE) -C Tutorial4  -f Makefile clean
	$(MAKE) -C Tutorial5  -f Makefile clean

# finis
