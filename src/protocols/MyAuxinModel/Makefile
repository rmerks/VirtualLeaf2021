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

all: myauxinmodel

myauxinmodel: Makefile.myauxinmodel
	$(MAKE) -f Makefile.myauxinmodel

Makefile.myauxinmodel: myauxinmodel.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.myauxinmodel clean
ifeq ($(MAKE),make)
	touch myauxinmodel.pro
else
	copy /b myauxinmodel.pro +,,
endif

# finis
