libjinventory:
-------
The libinventory package is a tool that harvests data from sysfs and presents the data
in JSON format on stdout. 
It supports CPU, network, and strorage devices.

Source:
-------
github.com/ibm/jinventory

Mailing list:
-------------

License:
--------
See 'LICENSE' file.

Compilation dependencies:
-------------------------
- C and C++ compiler (gcc, g++)
- GNU build tools (automake, autoconf, libtool, etc)
- yum: libudev-devel  Debian: libudev-dev
- yum: json-c-devel   Debian: libjson0 libjson0-dev

Binary dependencies:
-------------
  libudev & json-c .so 

  linking with other program -ljinventory

Building:
---------
$ autoreconf --install --force
$ ./configure
$ make
$ make install


