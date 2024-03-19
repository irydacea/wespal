Wesnoth RCX
===========

This is a small GUI application to allow [Wesnoth][1] artists and content
authors to easily preview graphics filtered through the game's team coloring
mechanism without needing to run the game itself.

[1]: <https://www.wesnoth.org/>

It provides a simple preview functionality, as well as the option to save the
results to disk for other purposes. A built-in visual palette and color range
editor allows artists and coders to play around with the various possibilities
offered by Wesnoth’s team coloring and generate WML code for use in add-ons.

License
-------

Wesnoth RCX as a whole is licensed under the GNU General Public License
version 2, or (at your option) any later version — a copy of the GNU GPL
version 2 is included in `COPYING`. Some parts include code licensed under
other licenses which may be found under `licenses/`.

Code from [KDE KImageFormats][2] licensed under the GNU Library General Public
License version 2 or later:

 * `src/kimageformats/util_p.h`

Code from [KDE KImageFormats][2] licensed under the GNU Lesser General Public
License version 2.1 or later:

 * `src/kimageformats/gimp_p.h`
 * `src/kimageformats/xcf.cpp`
 * `src/kimageformats/xcf_p.h`

[2]: <https://api.kde.org/frameworks/kimageformats/html/index.html>


Installing
----------

Windows and macOS users should use the binary packages provided on the
[Wesnoth RCX web page][3]. For Windows, no actual installation is required and
the provided .exe file will work out of the box and can be placed anywhere.

[3]: <https://irydacea.me/projects/wesnoth-rcx>

Users building from source should see the `INSTALL.md` file for instructions.


Usage
-----

To start RCX, just run the `wesnoth-rcx` executable. You’ll be asked to choose
an image file (PNG or BMP) to open. Once done, you’ll be presented with the
complete user interface, where you can preview your file under different
recoloring settings. You can also generate recolored copies of the original
image file this way.

The Tools menu allows access to the built-in color range and palette editors.
By default, there will be no ranges and palettes listed in them. Any items you
create will be stored in the application’s configuration. You can choose to
generate WML code for your user-defined palettes and ranges, and copy it to
clipboard, or simply save it to a file.


Configuration
-------------

* On Linux, the configuration will be stored in the file
  `$HOME/.config/Wesnoth/Morning Star.conf`

* On Windows, the configuration will be stored in the system Registry, under
  `HKEY_CURRENT_USER\Software\Wesnoth\Morning Star`

* On macOS, the configuration will be stored in the file
  `$HOME/Library/Preferences/com.Wesnoth.Morning Star.plist`


Reporting bugs
--------------

There is an issue tracker on [Github][4]. Alternatively, you may post in the
[Wesnoth.org forum topic][5].

[4]: https://github.com/irydacea/morningstar/issues
[5]: https://r.wesnoth.org/t31965

When reporting a bug, make sure to provide as much relevant information as
possible, including your operating system version (e.g. Windows 10 1903)
and Wesnoth RCX version. If you experience issues with a specific image or
palette/color range, be sure to provide those as well.
