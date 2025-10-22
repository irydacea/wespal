Changelog
=========

Version 0.5.2
-------------

### New features

### Bug fixes

### Other changes


Version 0.5.1
-------------

### New features

* Reworked the functionality to deselect all color ranges so instead it deselects all items other than the active color range, as well as ensures the active color range is selected if it isn't already.
* Dark mode UI is now supported on macOS.

### Bug fixes

* The image view no longer scrolls back to the center of the image when zooming in/out, selecting a different color range/palette/transform type, or closing Preferences (#14).
* Fixed QuaZip-related build errors with Qt 6.7.2 and later when using `ENABLE_BUILTIN_IMAGE_PLUGINS`.

### Other changes

* Windows and macOS versions built against Qt 6.9.3.


Version 0.5.0
-------------

### New features

* Out-of-the-box support for Krita (`.kra`), OpenRaster (`.ora`), and Adobe Photoshop (`.psd`) files on Windows and macOS via KImageFormats from KDE Frameworks 6.1.
* Redesigned main window:
  * Added buttons to quickly select or deselect all color ranges.
  * Added a button to generate image path functions for the open image.
  * Added an Edit menu and keyboard shortcuts for easy clipboard access.
  * Added alternative view modes to the original Split mode: Swipe, Onion Skin.
  * Added middle-mouse panning.
  * Added 1600% zoom factor.
* New settings dialog:
  * Added a General settings tab with display-related options.
  * Added option to disable saving the Wespal version number to created PNG files.
  * Merged Color Range and Palette editors into this dialog as separate tab pages.
  * Added an option to the Palette Editor to generate palette colors from the currently open image.
  * Color ranges now take a high-contrast fourth parameter, used in Wespal for displaying color range icons and in Wesnoth for unit/village markers on the minimap (#12).
  * Added options to the Color Range and Palette editors to create new items using Wesnoth built-ins as starting templates. This should allow for easier experimentation without having to manually copy the WML color lists into the From List dialog.
  * Added GIMP Palette (`.gpl`) export option to the Palette Editor.
* Added Wesnoth color blend (`~BLEND`) and color shift (`~CS`) functionality.
* Added Generate Base64 option to the Tools menu.

### Bug fixes

* Opening and closing a file persists the file's parent dir as the starting point of a new file Open operation (same session only).
* Saving recolored images persists the chosen dir as the starting point of a new file Save operation (same session only).
* Fixed Palette Editor not displaying any color range icons in the Recolor dropdown on macOS.
* Fixed saved PNGs not including a tEXt chunk with the Wespal version used to generate them as intended.
* Fixed Wespal application icon not being used even after installing on Wayland platforms.
* Added a workaround for increased issues with the KDE Plasma 6 Breeze style when dragging windows from any empty areas is enabled, by disabling dragging from "all" (actually most) areas of the main window instead. A proper fix will come eventually.
* Avoid a potential crash related to drop operations on Wayland.

### Other changes

* Windows and macOS versions built against Qt 6.7.0.
* Changed keys for the main window size and preview background settings. Note that this will cause these settings from version 0.4.0 and earlier to be forgotten.
* Made the code/WML snippet dialog a bit less cramped.
* Added CMake `ENABLE_BUILTIN_IMAGE_PLUGINS` option to enable building the built-in KImageFormats plugins and their QuaZip dependency. Users of platforms where KDE Frameworks 6 is readily available/already installed are discouraged from using this, so it's off by default.
* Fixed CMake `SANITIZE` option causing link-time errors (meaning yes, I actually tested and used it this time around).
* Implemented unit tests for backend functionality to ensure the recoloring algorithm never breaks silently before a release.


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
