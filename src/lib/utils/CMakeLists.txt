target_sources(santagger_lib
		PRIVATE
			utils_fmt.c
			w-base64.c
			w-base64-prot.h
			w-md5-prot.h
			utils_fmt-priv.h
			w-md5.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
