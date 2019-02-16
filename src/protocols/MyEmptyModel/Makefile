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

all: myemptymodel

myemptymodel: Makefile.myemptymodel
	$(MAKE) -f Makefile.myemptymodel

Makefile.myemptymodel: myemptymodel.pro
	$(QMAKE) -o $@ $< 

clean:
	$(MAKE) -f Makefile.myemptymodel clean
ifeq ($(MAKE),make)
	touch myemptymodel.pro
else
	copy /b myemptymodel.pro +,,
endif

# finis
