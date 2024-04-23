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
		../quazip
	)

    target_link_libraries(${plugin} PRIVATE
		Qt::Gui
	)
endfunction()

#
# kimg_xcf
#

kif_add_static_plugin(kimg_xcf CLASS_NAME XCFPlugin SOURCES
	gimp_p.h
	util_p.h
	xcf.cpp
	xcf_p.h
)

#
# kimg_psd
#

kif_add_static_plugin(kimg_psd CLASS_NAME PSDPlugin SOURCES
	fastmath_p.h
	psd.cpp
	psd_p.h
	util_p.h
)

#
# kimg_kra
#

kif_add_static_plugin(kimg_kra CLASS_NAME KraPlugin SOURCES
	kra.cpp
	kra.h
)

target_link_libraries(kimg_kra PRIVATE
	QuaZip::QuaZip
)

#
# kimg_ora
#

kif_add_static_plugin(kimg_ora CLASS_NAME OraPlugin SOURCES
	ora.cpp
	ora.h
)

target_link_libraries(kimg_ora PRIVATE
	QuaZip::QuaZip
)
