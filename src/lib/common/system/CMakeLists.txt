target_sources(santagger_lib
		PRIVATE
			sys_fnc.c
			sys_file.c
			sys_math.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
