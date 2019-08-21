Building Wesnoth RCX
====================

These instructions apply to the source code only. If you are using the provided
binary packages, you should be able to just run Wesnoth RCX right away.


Dependencies
------------

You will need a basic development environment (e.g. GCC with the C++ compiler)
as well as the Qt development files.

Wesnoth RCX should build and run with Qt 5.11 and later, and GCC 8 and later.
Slightly older versions may work as well but no support will be provided for
them.

KDE Frameworks 5 is not required to build or run Wesnoth RCX, but it may
provide additional image format plugins to handle layered GIMP `.xcf` and Adobe
Photoshop `.psd` files. If detected at runtime, Wesnoth RCX will support these
formats as well.


Building from source
--------------------

First you need to generate a Makefile:

```
$ cd wesnoth-rcx-X.Y.Z/
$ qmake -config release
```

Then you can build the application:

```
$ make
```

An executable file `wesnoth-rcx` will be generated in the current directory.


Installing
----------

For your convenience, the provided `install` target will install Wesnoth RCX
to `/usr/local/bin` and add an applications menu entry for you:

```
$ sudo make install
```

Or (as root):

```
# make install
```

If this does not suit your needs, you can copy the executable file generated in
the previous step to a different location, such as `$HOME/bin`, or just keep it
where it is — installation is *not* strictly necessary. However, you may want
to manually create a desktop or applications menu launcher for easy access.
