Building Wespal
===============

These instructions apply to the source code only. If you are using the provided binary packages, you should be able to just run Wespal right away.


Dependencies
------------

 * CMake 3.21.1 or later
 * GCC 7 or later / Clang 5 or later / another C++17-compatible compiler
 * Qt 6.4 or later

KDE Frameworks is not required to build or run Wespal, but if installed and properly configured, the KImageFormats component provides additional image format plugins to handle GIMP (`.xcf`), Krita (`.kra`), OpenRaster (`.ora`) and Adobe Photoshop (`.psd`) files.

> [!TIP]
> If you are running Linux and have KDE Plasma or KDE applications installed you will probably already have KImageFormats installed as well.

Alternatively, Wespal can use its own stripped-down version of KImageFormats if configured by CMake with `-DENABLE_BUILTIN_IMAGE_PLUGINS=ON`. In order for this to work, you will need to have the zlib development files installed as well.


Building from source
--------------------

> [!IMPORTANT]
> If you are checking out the source code from Git and wish to use the internal version of KImageFormats, you **must** make sure all Git submodules are checked out first by using the following commands. Failure to do so **will** result in build-time errors.
> ```
> $ git submodule init
> $ git submodule update
> ```

After unpacking the source code you need to set up the CMake build directory:

```
$ cd wespal-X.Y.Z/
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
```

Then you can build the application as follows. Adding `-jN` to the command line, replacing N with the number of CPU cores available (e.g. 4) can speed up compilation significantly:

```
$ make -j4
```

An executable file `wespal` will be generated in the build directory. You can either run Wespal straight from here, or follow the next step to install it system-wide to `/usr/local` or a custom location you may specify by including `-DCMAKE_INSTALL_PREFIX=/your/location/here` to CMake in step 1:

```
$ sudo make install
```

Installing Wespal system-wide has the added benefit of generating a desktop application menu entry for you. If you choose to run Wespal from its build directory you may want to grab a copy of the template in `x11/wespal.desktop` and edit it to point to the Wespal binary to make it easier to launch.


CMake options
-------------

The following options can be defined when configuring Wespal for building with CMake by specifying `-D<option>=<value>` in the command line (without the angle brackets):

* `CMAKE_BUILD_TYPE=<build type>`

  Selects the build type and optimization level, which is one of: `Debug` (default), `MinSizeRel`, `Release`, `RelWithDebInfo`. Most people want to use `Release`.

* `ENABLE_TESTS`

  Enables a test suite to be built for development purposes.

* `ENABLE_BUILTIN_IMAGE_PLUGINS`

  Enables an internal stripped-down version of KImageFormats to be built in order to support additional image formats. If you have KDE Frameworks 6 installed, it is highly recommended you leave this option disabled.

* `SANITIZE=<compiler specific>`

  Enables compiler `-fsanitize` instrumentation. Example: `cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=address,leak`
