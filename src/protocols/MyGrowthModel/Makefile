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

all: mygrowthmodel

mygrowthmodel: Makefile.mygrowthmodel
	$(MAKE) -f Makefile.mygrowthmodel

Makefile.mygrowthmodel: mygrowthmodel.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.mygrowthmodel clean
ifeq ($(MAKE),make)
	touch mygrowthmodel.pro
else
	copy /b mygrowthmodel.pro +,,
endif

# finis
