# Development scripts

This directory contains utilities used for development, deployment or testing of Wespal. Some of them may be specifically written for my system and assume things about the operating environment that will not be true on your machine. **USE AT YOUR OWN RISK.**

(Note that if you do use them, you will probably need to tweak some of the file or directory references in them to suit your environment.)

* `build-icons`: Used to generate Windows `.ico` and macOS `.icns` files
* `miniqt-windows.cmd`: Used to build a minimal statically-linked version of Qt for Windows (Mingw only)
* `dist-mac`: Used to build the macOS bundle and .dmg image
* `dist-src`: Used to generate the source code release tarball
* `dist-windows.cmd`: Used to build the Windows distribution
