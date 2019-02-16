.. $Id$

.. |date| date::
.. |time| date:: %H:%M

.. header::
  ###Title###  -  |date|

.. footer::
  ###Page### 

=========================
Installation Instructions
=========================

Requirements
------------

The VirtualLeaf is written with the QT Cross platform application and
UI framework, and can run on Linux, Apple Macintosh and Windows
machines. To compile the VirtualLeaf from source you will need to
install the QT software development kit(SDK) which you can download
from the `QT download site <http://qt.nokia.com/downloads/downloads>`_.
Whether installed globally by a system administrator our locally in
your own user space, make sure the QT bin directory containing qmake
is in your execution path. Platform specific instructions follow.


All Platforms
-------------

Compilation is effected with make, either the native ``make`` on UNIX
and MAC systems or ``mingw32-make`` distributed with the windows
version of QT. In the VirtualLeaf ``src`` directory you will find a
``Makefile``, the root of a hierarchy of makefiles, that will guide
the compilation and installation of the VirtualLeaf executable, its
default plugins and the tutorial examples.  To facilitate cross
platform compatibility, the makefiles use two environment variables:
``MAKE`` and ``QMAKE`` to name the make and QT-make utility
respectively. The fallback defaults are ``make`` and
``qmake``. Platform specific instructions follow.


Linux
-----

Open the terminal window and unpack the VirtualLeaf source package
into your home folder, by typing::

 > cd 
 > tar xzf VirtualLeaf-V1.0.tgz


So the Makefiles can find ``qmake``, prepend the QT bin directory to
your execution path, for example::

 > PATH=/opt/QT/qt/bin:$PATH 

If you wish to use some other ``make`` or ``qmake`` utilities other
than the defaults, instantiate environment variables named ``MAKE``
and ``QMAKE``, for example::

 > export MAKE=gmake
 > export QMAKE=/home/michael/MyQT/qt/bin/qmake

To compile the VirtualLeaf, go to the VirtualLeaf ``src`` directory
and invoke ``make``, for example::

 > cd /home/michael/VirtualLeaf/v1.0/src
 > make -f Makefile

Once the compilation is complete, you will find the ``VirtualLeaf``
binary in ``v1.0/bin`` and the plugins in ``v1.0/bin/models``.


Windows
-------

Unpack the VirtualLeaf source package somewhere in your folder
hierarchy. The result might be in, for example: ``c:\Documents and
Settings\michael\VirtualLeaf\v1.0``.

For convenience sake, three required development libraries:
``libiconv``, ``libxml2`` and ``libz`` are distributed with the
VirtualLeaf code, and the VirtualLeaf will compile correctly with
them. If, however, you wish to use other versions of these libraries,
you will need to reassign the ``LIBICONVDIR``, ``LIBZML2DIR`` and
``LIBZDIR`` variables in all the project files.

As stated earlier, the Makefiles rely on two environment variables:
``MAKE`` and ``QMAKE``.  Since QT executable names are consistent
across platforms, the ``QMAKE`` default value, ``QMAKE=qmake``, will
work fine. You will have to create an environment named ``MAKE``
though.

To add an environment variable call up the *system tool*, either with
a left click on the *start menu* then a right click on *My Computer*
and choosing *properties* from the drop down list, or by choosing
*control panel* from the *start menu* and then double clicking on the
*system tool* itself. Once in the *system tool*, choose the *advanced*
tab and then click the *environment variables* button. The environment
variables screen is split in two with *User* variables above and
*System* variables below. Add a new users variable named ``MAKE`` with
the value, e.g. ``mingw32-make``. the QT(SDK) contains the *minimum
gnu for windows* (mingw) compiler; commonly installed in:
``c:\QT\qt-version\mingw\bin``. Look there for the ``MAKE`` utility
name.

After installing QT and setting the ``MAKE`` environment variable, you
should be able to invoke a QT command window from the *start
menu*. This shell automatically includes the necessary QT folder in
your execution PATH. Within this command window, go to the VirtualLeaf
``src`` folder and invoke make, for example::

 > c:\Documents and Settings\michael\VirtualLeaf\v1.0\src
 > mingw32-make -f Makefile

When complete, you will find the ``VirtualLeaf.exe`` binary in
``v1.0/bin`` and the plugins in ``v1.0/bin/models``.


MacOS
-----

Make sure that XCode development environment is installed from 
the MacOSX installations DVDs. Also download and install a recent 
version of Qt as described above (version >4.4).

Open the Terminal program (in ``/Applications/Utilities/``). 
Unpack the VirtualLeaf source package into your home folder,
by typing::

 > cd 
 > tar xzf VirtualLeaf-V1.0.tgz

Now go to the VirtualLeaf ``src`` directory, set the right options for qmake,
and invoke make, by typing, for example::

 > cd 
 > cd VirtualLeaf/v1.0/src
 > export QMAKESPEC=macx-g++; make -f Makefile

When complete, you will find the ``VirtualLeaf`` binary in
``v1.0/bin`` and the plugins in ``v1.0/bin/models``.

