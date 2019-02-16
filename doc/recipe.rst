.. $Id$

.. |date| date::
.. |time| date:: %H:%M

.. header::
  ###Title###  -  |date|

.. footer::
  ###Page### 

============
Virtual Leaf
============

------------------
Linux Installation
------------------

Material
````````
#. The Virtual Leaf source code from http://virtualleaf.googlecode.com/files/VirtualLeaf-v1.0-src.zip.
#. The QT SDK from http://qt.nokia.com.
#. Development Systems will have many of these items already installed:

  #. KDiff2 diff/merge tool.
  #. GCC, The GNU Compiler Collection
  #. GNU Make
  #. Iconv conversion utilities
  #. Perl programming language
  #. Zlib compression library
  #. Libxml2 XML parser

Methods
```````

#. Install the aforementioned development software. If any of these
   are missing, ask your system administrator to install them for
   you. The following commands should show meaningful output::

    gcc --version 
    make --version
    iconv --version
    perl --version
    locate libz.so
    locate libxml2.so


#. The VirtualLeaf is written with the QT Cross platform application
   and UI framework. To compile the VirtualLeaf from source you will
   need to install the QT software development kit(SDK), which you
   can download from the QT download site: ``http://qt.nokia.com/downloads/downloads>``.

#. Whether installed globally by a system administrator our locally in
   your own user space, make sure the QT bin directory containing qmake
   is in your execution path, e.g.::

    > PATH=/opt/QT/qt/bin:$PATH 

#. Make or change to the directory where you wish to install the Virtual Leaf::

    > cd /home/michael/simulations

#. Retrieve the Virtual Leaf source code from its  Google Code repository::

    > wget http://virtualleaf.googlecode.com/files/VirtualLeaf-v1.0-src.zip
    > unzip http://virtualleaf.googlecode.com/files/VirtualLeaf-v1.0-src.zip

#. Compilation is effected with make. To facilitate cross platform
   compatibility, the makefiles use two environment variables:
   ``MAKE`` and ``QMAKE`` to name the make and QT-make utilities
   respectively. The fallback defaults are ``make`` and ``qmake``. 
   If you wish to use some other ``make`` or ``qmake`` utilities other
   than the defaults, instantiate environment variables named ``MAKE``
   and ``QMAKE``, e.g.::

    > export MAKE=gmake
    > export QMAKE=/home/michael/MyQT/qt/bin/qmake

#. In the VirtualLeaf ``src`` directory you will find a
   ``Makefile``, the root of a hierarchy of makefiles, that will
   guide the compilation and installation of the VirtualLeaf
   executable, its default plugins and the tutorial examples.  To
   compile the VirtualLeaf**, go to the VirtualLeaf ``src`` directory
   and invoke ``make``, for example::

    > cd /home/michael/simulations/virtualleaf/src
    > make -f Makefile

   Once the compilation is complete, you will find the ``VirtualLeaf``
   binary in ``virtualleaf/bin`` and the plugins in ``virtualleaf/bin/models``.

--------------------
Windows Installation
--------------------

Material
````````
#. The Virtual Leaf source code from http://virtualleaf.googlecode.com/files/VirtualLeaf-v1.0-src.zip.

#. The QT SDK from http://get.qt.nokia.com/qtsdk/qt-sdk-win-opensource-2010.02.1.exe.

#. Development Systems most likely will have most of the items in the following list already installed:

   #. KDiff3 diff/merge tool http://sourceforge.net/projects/kdiff3/files.

   #. Iconv utilities http://sourceforge.net/projects/mingw/files/MinGW/libiconv/libiconv-1.13 (rename libiconv.dll.a to libiconv.a).

   #. Perl http://strawberry-perl.googlecode.com/files/strawberry-perl-5.10.1.1.msi.

   #. Zlib library ftp://sourceforge.net/projects/mingw/files/MinGW/zlib/zlib-1.2.3-1-mingw32.

   #. libz-1.dll  http://sourceforge.net/projects/mingw/files/MinGW/zlib/zlib-1.2.3-1-mingw32 (rename to zlib1.dll).

   #. Libxml2 XML parser ftp://ftp.zlatkovic.com/libxml or ftp://xmlsoft.org/libxml2/win32. 

      Or compile from source::

        git clone git://git.gnome.org/libxml2

      To obviate compilation problems with mingw see:
       #. https://bugzilla.gnome.org/show_bug.cgi?id=561340 
       #. http://osdir.com/ml/gnome.lib.xml.general/2004-02/msg00007.html
       #. http://www.c-plusplus.de/forum/viewtopic-var-t-is-260421.html 
       #. http://markmail.org/message/rlpeuruibg2ruqqv 

Methods
```````
#. Install the aforementioned development software. If any of these
   are missing, ask your system administrator to install them for
   you.

#. The VirtualLeaf is written with the QT Cross platform application
   and UI framework. To compile the VirtualLeaf from source you will
   need to install the QT software development kit(SDK), which you
   can download from the `QT download site <http://qt.nokia.com/downloads/downloads>`_.

#. Make or change to the folder where you wish to install the Virtual Leaf::

    > c:\Documents and Settings\michael\simulations

#. Retrieve the Virtual Leaf source code from its Google Code
   repository by typing
   http://virtualleaf.googlecode.com/files/VirtualLeaf-v1.0-src.zip
   into your browser. Unpack the archive and place its contents in
   the folder named in the previous step.


#. Compilation is effected with make. To facilitate cross platform
   compatibility, the makefiles use two environment variables:
   ``MAKE`` and ``QMAKE`` to name the make and QT-make utility
   respectively. The fallback defaults are ``make`` and
   ``qmake``. Since QT executable names are consistent across
   platforms, the ``QMAKE`` default value, ``QMAKE=qmake``, will
   work fine. You will have to create an environment variable named ``MAKE``
   though.

   To add an environment variable call up the *system tool*, either
   with a left click on the *start menu* then a right click on *My
   Computer* and choosing *properties* from the drop down list, or
   by choosing *control panel* from the *start menu* and then double
   clicking on the *system tool* itself. Once in the *system tool*,
   choose the *advanced* tab and then click the *environment
   variables* button. The environment variables screen is split in
   two with *User* variables above and *System* variables below. Add
   a new users variable named ``MAKE`` with the value,
   e.g. ``mingw32-make``. the QT(SDK) contains the *minimum gnu for
   windows* (mingw) compiler; commonly installed in:
   ``c:\QT\qt-version\mingw\bin``. Look there for the ``MAKE``
   utility name.

#. For convenience sake, three required development libraries:
   ``libiconv``, ``libxml2`` and ``libz`` are distributed with the
   VirtualLeaf code, and the VirtualLeaf will compile correctly with
   them. If, however, you wish to use other versions of these
   libraries, you will need to reassign the ``LIBICONVDIR``,
   ``LIBZML2DIR`` and ``LIBZDIR`` variables in all the project
   files.

#. After installing QT and setting the ``MAKE`` environment
   variable, you should be able to invoke a QT command window from
   the *start menu*. This shell automatically includes the necessary
   QT folder in your execution PATH. Within this command window, go
   to the VirtualLeaf ``src`` folder and invoke make, for example::

     > c:\Documents and Settings\michael\simulations\virtualleaf\src
     > mingw32-make -f Makefile

   When complete, you will find the ``VirtualLeaf.exe`` binary in
   ``v1.0/bin`` and the plugins in ``v1.0/bin/models``.

------------------
MacOS Installation
------------------

Material
````````

Identical to the Linux installation except that instead of GNU GCC
and Make, install the XCode development environment from the MacOSX
installation DVDs.


Methods
```````

Identical to the Linux installation except:
  #. Open the Terminal program (in ``/Applications/Utilities/``) and
     retrieve the Virtual Leaf source code from its Google Code
     repository::

       > hg clone https://virtualleaf.googlecode.com/hg/ virtualleaf

  #. Compile the VirtualLeaf code with the correct options::

       > cd virtualleaf/src
       > export QMAKESPEC=macx-g++; make -f Makefile

     When complete, you will find the ``VirtualLeaf`` binary in
     ``v1.0/bin`` and the plugins in ``v1.0/bin/models``.


