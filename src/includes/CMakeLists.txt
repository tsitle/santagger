target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			lib_version.h
			lib_defs.h
	)

add_subdirectory("audvid")
add_subdirectory("utils")
add_subdirectory("common")
add_subdirectory("tag")
