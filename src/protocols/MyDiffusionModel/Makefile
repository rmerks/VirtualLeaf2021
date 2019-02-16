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

all: mydiffusionmodel

mydiffusionmodel: Makefile.mydiffusionmodel
	$(MAKE) -f Makefile.mydiffusionmodel

Makefile.mydiffusionmodel: mydiffusionmodel.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.mydiffusionmodel clean
ifeq ($(MAKE),make)
	touch mydiffusionmodel.pro
else
	copy /b mydiffusionmodel.pro +,,
endif

# finis
