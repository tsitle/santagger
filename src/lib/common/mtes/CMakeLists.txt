target_sources(santagger_lib
		PRIVATE
			w-utf-prot.h
			string_mte-priv.h
			string_mte.c
			w-utf.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
