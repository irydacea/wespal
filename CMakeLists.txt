cmake_minimum_required(VERSION 3.21.1)
project(wespal VERSION 0.5.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

#
# Optional build-time settings
#

set(SANITIZE "" CACHE STRING "Comma-separated list of compiler -fsanitize instrumentation to enable")
option(ENABLE_TESTS "Build unit tests")

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
endif()

#
# Dependency/Qt and compiler setup
#

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui Widgets OPTIONAL_COMPONENTS Test)

qt_standard_project_setup()

function(kif_add_static_plugin plugin)
	set(options)
	set(oneValueArgs CLASS_NAME)
	set(multiValueArgs SOURCES)

	cmake_parse_arguments(KIF_ADD_PLUGIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT KIF_ADD_PLUGIN_CLASS_NAME)
		message(FATAL_ERROR "kif_add_static_plugin called without CLASS_NAME parameter")
	endif()

	if(NOT KIF_ADD_PLUGIN_SOURCES)
		message(FATAL_ERROR "kif_add_static_plugin called without SOURCES parameter")
	endif()

	qt_add_plugin(${plugin} STATIC PLUGIN_TYPE imageformats CLASS_NAME ${KIF_ADD_PLUGIN_CLASS_NAME})

	target_sources(${plugin} PRIVATE
		${KIF_ADD_PLUGIN_SOURCES}
	)

    target_include_directories(${plugin} SYSTEM PRIVATE
		src/3rdparty/quazip
	)

    target_link_libraries(${plugin} PRIVATE
		Qt::Gui
	)
endfunction()

#
# QuaZip
#

set(BUILD_SHARED_LIBS OFF)
set(QUAZIP_QT_MAJOR_VERSION 6)
set(QUAZIP_BZIP2 OFF)
set(QUAZIP_INSTALL OFF)

add_subdirectory(src/3rdparty/quazip)

#
# KImageFormats (kimg_xcf, kimg_kra, kimg_ora)
#

kif_add_static_plugin(kimg_xcf CLASS_NAME XCFPlugin SOURCES
	src/3rdparty/kimageformats/gimp_p.h
	src/3rdparty/kimageformats/util_p.h
	src/3rdparty/kimageformats/xcf.cpp
	src/3rdparty/kimageformats/xcf_p.h
)

kif_add_static_plugin(kimg_kra CLASS_NAME KraPlugin SOURCES
	src/3rdparty/kimageformats/kra.cpp
	src/3rdparty/kimageformats/kra.h
)

target_link_libraries(kimg_kra PRIVATE
	QuaZip::QuaZip
)

kif_add_static_plugin(kimg_ora CLASS_NAME OraPlugin SOURCES
	src/3rdparty/kimageformats/ora.cpp
	src/3rdparty/kimageformats/ora.h
)

target_link_libraries(kimg_ora PRIVATE
	QuaZip::QuaZip
)

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
	# Windows resource file, including native icon
	set(wespal_platform_files win32/wespal.rc)
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
	MACOSX_BUNDLE_COPYRIGHT "© 2008 - 2024 by Iris Morelle"
	MACOSX_BUNDLE_ICON_FILE Wespal.icns
)

qt_import_plugins(wespal INCLUDE
	kimg_xcf
	kimg_kra
	kimg_ora
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
	kimg_xcf
	kimg_kra
	kimg_ora
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
