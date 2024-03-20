cmake_minimum_required(VERSION 3.16)
project(wesnoth-rcx VERSION 0.4 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_plugin(xcfplugin STATIC CLASS_NAME XCFPlugin
	src/kimageformats/gimp_p.h
	src/kimageformats/util_p.h
	src/kimageformats/xcf.cpp src/kimageformats/xcf_p.h
)

target_link_libraries(xcfplugin PRIVATE
	Qt::Core
	Qt::Gui
)

qt_add_executable(wesnoth-rcx WIN32 MACOSX_BUNDLE
	src/appconfig.cpp src/appconfig.hpp
	src/codesnippetdialog.cpp src/codesnippetdialog.hpp src/codesnippetdialog.ui
	src/colorlistinputdialog.cpp src/colorlistinputdialog.hpp src/colorlistinputdialog.ui
	src/colortypes.hpp
	src/custompalettes.cpp src/custompalettes.hpp src/custompalettes.ui
	src/customranges.cpp src/customranges.hpp src/customranges.ui
	src/defs.cpp src/defs.hpp
	src/imagelabel.cpp src/imagelabel.hpp
	src/main.cpp
	src/mainwindow.cpp src/mainwindow.hpp src/mainwindow.ui
	src/paletteitem.cpp src/paletteitem.hpp
	src/recentfiles.cpp src/recentfiles.hpp
	src/util.cpp src/util.hpp
	src/version.cpp src/version.hpp
	src/wesnothrc.cpp src/wesnothrc.hpp
)

qt_import_plugins(wesnoth-rcx INCLUDE XCFPlugin)

target_compile_options(wesnoth-rcx PRIVATE
	-Wall
	-Wextra
	-Werror=non-virtual-dtor
	-Wold-style-cast
)

target_compile_definitions(wesnoth-rcx PRIVATE
	QT_NO_FOREACH
)

target_link_libraries(wesnoth-rcx PRIVATE
	Qt::Core
	Qt::Gui
	Qt::Widgets
	xcfplugin
)

# Resources:
set_source_files_properties("icons/128x128/wesnoth-rcx.png"
	PROPERTIES QT_RESOURCE_ALIAS "wesnoth-rcx-icon-128.png"
)
set_source_files_properties("icons/16x16/edit-copy.png"
	PROPERTIES QT_RESOURCE_ALIAS "edit-copy-16.png"
)
set_source_files_properties("icons/16x16/list-add.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-add-16.png"
)
set_source_files_properties("icons/16x16/list-remove.png"
	PROPERTIES QT_RESOURCE_ALIAS "list-remove-16.png"
)
set_source_files_properties("icons/16x16/zoom-in.png"
	PROPERTIES QT_RESOURCE_ALIAS "zoom-in-16.png"
)
set_source_files_properties("icons/16x16/zoom-out.png"
	PROPERTIES QT_RESOURCE_ALIAS "zoom-out-16.png"
)
set_source_files_properties("icons/256x256/wesnoth-rcx.png"
	PROPERTIES QT_RESOURCE_ALIAS "wesnoth-rcx-icon-256.png"
)
set_source_files_properties("icons/64x64/wesnoth-rcx.png"
	PROPERTIES QT_RESOURCE_ALIAS "wesnoth-rcx-icon-64.png"
)
set(morningstar_resource_files
	"icons/128x128/wesnoth-rcx.png"
	"icons/16x16/edit-copy.png"
	"icons/16x16/list-add.png"
	"icons/16x16/list-remove.png"
	"icons/16x16/zoom-in.png"
	"icons/16x16/zoom-out.png"
	"icons/256x256/wesnoth-rcx.png"
	"icons/64x64/wesnoth-rcx.png"
)

qt_add_resources(wesnoth-rcx "morningstar"
	PREFIX
		"/"
	FILES
		${morningstar_resource_files}
)

install(TARGETS wesnoth-rcx
	BUNDLE DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
	TARGET wesnoth-rcx
	FILENAME_VARIABLE deploy_script
	NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
