cmake_minimum_required(VERSION 3.21.1)
project(wespal VERSION 0.5.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

#
# Optional build-time settings
#

set(SANITIZE "" CACHE STRING "Comma-separated list of compiler -fsanitize instrumentation to enable")
option(ENABLE_TESTS "Build unit tests")
option(ENABLE_BUILTIN_IMAGE_PLUGINS "Builds and enables bundled versions of KDE Frameworks plugins for image format support" OFF)

set(cxx_sanitizer_flags "")
if(SANITIZE)
	set(cxx_sanitizer_flags "-fsanitize=${SANITIZE}")
endif()

set(cxx_warning_flags "")
if(NOT MSVC)
	set(cxx_warning_flags
		-Wall
		-Wextra
		-Werror=non-virtual-dtor
		-Wold-style-cast
	)
else()
	set(cxx_warning_flags
		/W3
	)
endif()

set(wespal_builtin_image_plugins "")

#
# Dependency/Qt and compiler setup
#


if(ENABLE_BUILTIN_IMAGE_PLUGINS)
	find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Core5Compat)
	find_package(Qt6 REQUIRED COMPONENTS Gui Widgets Core5Compat OPTIONAL_COMPONENTS Test)
else()
	find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
	find_package(Qt6 REQUIRED COMPONENTS Gui Widgets OPTIONAL_COMPONENTS Test)
endif()

qt_standard_project_setup()

#
# Third-party support libraries
#

add_subdirectory(src/3rdparty)

#
# libmorningstar
#

qt_add_library(morningstar STATIC
	src/colortypes.hpp
	src/defs.cpp src/defs.hpp
	src/recentfiles.cpp src/recentfiles.hpp
	src/version.cpp src/version.hpp
	src/wesnothrc.cpp src/wesnothrc.hpp
)

target_link_libraries(morningstar PRIVATE
	Qt::Core
	Qt::Gui
)

target_compile_options(morningstar PRIVATE
	${cxx_warning_flags}
	${cxx_sanitizer_flags}
)

target_link_options(morningstar PRIVATE
	${cxx_sanitizer_flags}
)

#
# Test suite
#

if(ENABLE_TESTS)
	qt_add_executable(wespal_tests WIN32
		src/tests.cpp src/tests.hpp
	)

    target_compile_options(wespal_tests PRIVATE
		${cxx_warning_flags}
		${cxx_sanitizer_flags}
	)

	target_link_options(wespal_tests PRIVATE
		${cxx_sanitizer_flags}
	)

    target_link_libraries(wespal_tests PRIVATE
		Qt::Core
		Qt::Gui
		Qt::Test
		Qt::Widgets
		morningstar
	)

    enable_testing()

	add_test(NAME wespal_tests COMMAND
		wespal_tests -platform offscreen
	)
endif()

#
# Wespal
#

if(WIN32)
	# vcpkg manifest + Windows resource file including native icon
	set(wespal_platform_files win32/wespal.rc vcpkg.json)
elseif(APPLE)
	# macOS native icon
	set(wespal_platform_files macos/Wespal.icns)
	set_source_files_properties(${wespal_platform_files} PROPERTIES
		MACOSX_PACKAGE_LOCATION "Resources")
else()
	set(wespal_platform_files "")
endif()

qt_add_executable(wespal WIN32 MACOSX_BUNDLE
	src/appconfig.cpp src/appconfig.hpp
	src/codesnippetdialog.cpp src/codesnippetdialog.hpp src/codesnippetdialog.ui
	src/colorlistinputdialog.cpp src/colorlistinputdialog.hpp src/colorlistinputdialog.ui
	src/compositeimagelabel.cpp src/compositeimagelabel.hpp
	src/imagelabel.cpp src/imagelabel.hpp
	src/settingsdialog.hpp src/settingsdialog.cpp src/settingsdialog.ui
	src/mainwindow.cpp src/mainwindow.hpp src/mainwindow.ui
	src/paletteitem.cpp src/paletteitem.hpp
	src/util.cpp src/util.hpp
	${wespal_platform_files}
	src/main.cpp
)

set_target_properties(wespal PROPERTIES
	MACOSX_BUNDLE_BUNDLE_NAME "Wespal"
	MACOSX_BUNDLE_GUI_IDENTIFIER "me.irydacea.Wespal"
	MACOSX_BUNDLE_COPYRIGHT "© 2008 - 2025 by Iris Morelle"
	MACOSX_BUNDLE_ICON_FILE Wespal.icns
)

qt_import_plugins(wespal INCLUDE
	${wespal_builtin_image_plugins}
)

target_compile_definitions(wespal PRIVATE
	QT_NO_FOREACH
)

target_compile_options(wespal PRIVATE
	${cxx_warning_flags}
	${cxx_sanitizer_flags}
)

target_link_options(wespal PRIVATE
	${cxx_sanitizer_flags}
)

target_link_libraries(wespal PRIVATE
	Qt::Core
	Qt::Gui
	Qt::Widgets
	${wespal_builtin_image_plugins}
	morningstar
)

# Resources:
set_source_files_properties("icons/private/edit-copy.png"
	PROPERTIES QT_RESOURCE_ALIAS "edit-copy-16.png"
)
set_source_files_properties("icons/private/edit-copy@2x.png"
	PROPERTIES QT_RESOURCE_ALIAS "edit-copy-16@2x.png"
)
set_source_files_properties("icons/private/list-add.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-add-16.png"
)
set_source_files_properties("icons/private/list-add@2x.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-add-16@2x.png"
)
set_source_files_properties("icons/private/list-remove.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-remove-16.png"
)
set_source_files_properties("icons/private/list-remove@2x.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-remove-16@2x.png"
)
set_source_files_properties("icons/hicolor/16x16/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-16.png"
)
set_source_files_properties("icons/hicolor/32x32/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-32.png"
)
set_source_files_properties("icons/hicolor/48x48/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-48.png"
)
set_source_files_properties("icons/hicolor/64x64/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-64.png"
)
set_source_files_properties("icons/hicolor/128x128/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-128.png"
)
set_source_files_properties("icons/hicolor/256x256/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-256.png"
)
set_source_files_properties("icons/hicolor/512x512/apps/wespal.png"
	PROPERTIES QT_RESOURCE_ALIAS "wespal-icon-512.png"
)
set(wespal_resource_files
	"icons/private/edit-copy.png"
	"icons/private/edit-copy@2x.png"
	"icons/private/list-add.png"
	"icons/private/list-add@2x.png"
	"icons/private/list-remove.png"
	"icons/private/list-remove@2x.png"
	"icons/hicolor/16x16/apps/wespal.png"
	"icons/hicolor/32x32/apps/wespal.png"
	"icons/hicolor/48x48/apps/wespal.png"
	"icons/hicolor/64x64/apps/wespal.png"
	"icons/hicolor/128x128/apps/wespal.png"
	"icons/hicolor/256x256/apps/wespal.png"
	"icons/hicolor/512x512/apps/wespal.png"
)

qt_add_resources(wespal "wespal"
	PREFIX "/"
	FILES ${wespal_resource_files}
)

#
# Deployment
#

install(TARGETS wespal
	BUNDLE DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES desktop/me.irydacea.Wespal.desktop
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

# Only wespal.png is public
install(DIRECTORY icons/hicolor
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons
	FILES_MATCHING PATTERN "wespal.png"
)
