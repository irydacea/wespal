Building Wespal
===============

These instructions apply to the source code only. If you are using the provided binary packages, you should be able to just run Wespal right away.


Dependencies
------------

You will need a basic development environment (e.g. GCC with the C++ compiler) as well as the Qt development files.

Wespal should build and run with Qt 6.4 and later, and any compiler with decent C++17 support. Slightly older versions may work as well but no support will be provided for them.

KDE Frameworks is not required to build or run Wespal, but if installed and properly configured, the KImageFormats component may provide additional image format plugins to handle Krita (`.kra`), OpenRaster (`.ora`) and Adobe Photoshop (`.psd`) files.


Building from source
--------------------

First, after unpacking the source code you need to set up the CMake build directory:

```
$ cd wespal-X.Y.Z/
$ mkdir build
$ cd build
$ cmake ..
```

Then you can build the application:

```
$ make
```

An executable file `wespal` will be generated in the current directory.


Installing
----------

For your convenience, the provided `install` target will install Wespal to `/usr/local/bin` and add an applications menu entry for you:

```
$ sudo make install
```

Or (as root):

```
# make install
```

If this does not suit your needs, you can copy the executable file generated in the previous step to a different location, such as `$HOME/bin`, or just keep it where it is — installation is *not* strictly necessary. However, you may want to manually create a desktop or applications menu launcher for easy access.
