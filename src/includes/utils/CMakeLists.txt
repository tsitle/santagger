target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			utils_fmt-defs.h
			utils_fmt.h
	)
