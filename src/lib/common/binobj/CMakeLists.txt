target_sources(santagger_lib
		PRIVATE
			binobj-prot.h
			binobj_stcs.c
			binobj.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
