Changelog
=========

Version 0.5.0
-------------

* Added alternative view modes to the original Split mode: Swipe, Onion Skin.
* Added an option to generate palette colors from the currently open image.
* Added middle-mouse panning to image previews.
* Added 1600% zoom factor.
* Redesigned main window.
* Merged Color Range Editor and Palette Editor into a single Settings window with additional global settings.
* Changed storage for the main window size and preview background settings - this will cause these settings from previous versions to be forgotten.
* Added options to the Color Range and Palette Editor to create new items using Wesnoth built-ins as starting templates. This should allow for easier experimentation without having to manually copy the WML color lists into the From List dialog.
* Added GIMP Palette (.gpl) export option to the Palette Editor.
* Fixed Palette Editor not displaying any color range icons in the Recolor dropdown on macOS.
* Color ranges now take a high-contrast fourth parameter, used in Wespal for displaying color range icons and in Wesnoth for unit/village markers on the minimap (#12).
* Added Generate Base64 option to the Tools menu.
* Added unit tests to ensure the recoloring algorithm never breaks silently before a release.

Version 0.4.0
-------------

* New icon.
* Rebranded to Wespal.
* Now requires Qt 6.4 or later, CMake 3.21.1 or later, GCC 7 or later / Clang 5 or later / another C++17-compatible compiler.
* WebP file format enabled where supported.
* XCF image format plugin from KImageFormats is included and enabled for all platforms to avoid depending on KDE Frameworks runtime.
* It is now possible to close the active file without quitting the app.
* Not opening a file on startup no longer causes the app to quit.
* Various changes to make the app feel more native on macOS.
* Color Range Editor:
  * Added a "From List" option to allow copying color values straight from WML.
  * Fixed right side panel still displaying the colors of the deleted range
    after deleting a range.
* Increased Recent Files menu size from 4 to 8.
* New start screen workflow including a more visible display of recently-opened
  files.

Version 0.3.0
-------------

* Ported to Qt 5 and C++11.
* Added more unit team colors introduced in Wesnoth 1.14 (light red, dark
   red, light blue, bright green, bright orange, gold).
* Palette Editor: Update palette icon after recoloring.
* Main: Zoom view in/out using Ctrl++ (plus) or Ctrl+- (minus).
* Main: Zoom view in/out using Ctrl+<vertical scrollwheel>.
* Main: Made it possible for the user to choose any custom color for the
  preview background.

Version 0.2.1
-------------

* Compatibility fixes for Qt 4.6 and 4.7.
* Add Colors dialog: Minor visual improvements
* Main: Make preview background color setting persistent.
* Main: Remember the main window size for subsequent runs.
* Main: Solve excessive memory usage when zooming in. This can save around
  1 GiB of RAM in some cases (!).
* Palette Editor: "Add from List" should stay disabled when there are no
  available palettes.
* Palette Editor: Added option to apply a color range (team coloring) to
  the current selected palette.

Version 0.2.0
-------------

* Added an embedded application icon and version information to Windows
  builds.
* Added various preview background color options.
* Implemented built-in editors for custom color ranges and palettes,
  as well as options to export individual items as WML (either to a
  new file, or the system clipboard).
* Implemented Reload (F5) action.
* Implemented Recent Files list in the File menu.
* Redesigned the main window.

Version 0.1.4
-------------

* Complete drag-and-drop support.
* Implemented basic zoom support.

Version 0.1.3
-------------

* Fix an off-by-one bug causing problems with the TC recolor jobs.


Version 0.1.2
-------------

* Added support for BMP, PSD* and XCF* files (* indicates an option that
  depends on Qt4's build-time configuration.)
* Drop support for image files and data from other applications.
* Minor UI improvements or revisions.


Version 0.1.1
-------------

* Preliminary production name: “Wesnoth RCX” (a.k.a. RCX).
* Made the main window resizable again — now widgets expand or shrink
  accordingly.
* Added generated file paths to completed job reports.
* Fixed some glitches with selection changes in the color ranges listbox.
* Minor UI improvements or revisions.

Version 0.1.0.1
---------------

* Fixed a bogus message when canceling the Open action.
* Made the window non-resizable since widgets don’t adjust to it yet.
* Previews are now centered.
* Correctly remember the path to the currently opened file.

Version 0.1.0
-------------

* Initial version.
