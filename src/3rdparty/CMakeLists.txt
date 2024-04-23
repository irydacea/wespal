if(ENABLE_BUILTIN_IMAGE_PLUGINS)
	#
	# QuaZip
	#

	set(BUILD_SHARED_LIBS OFF)
	set(QUAZIP_QT_MAJOR_VERSION 6)
	set(QUAZIP_BZIP2 OFF)
	set(QUAZIP_INSTALL OFF)

	add_subdirectory(quazip)

	#
	# KImageFormats (kimg_xcf, kimg_kra, kimg_ora)
	#

	add_subdirectory(kimageformats)

	set(wespal_builtin_image_plugins
		kimg_xcf
		kimg_psd
		kimg_kra
		kimg_ora
		PARENT_SCOPE
	)
endif()
