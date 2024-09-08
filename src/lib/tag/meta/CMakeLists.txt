target_sources(santagger_lib
		PRIVATE
			tag_meta_gs.c
			tag_meta.c
			tag_meta_fnc-prot.h
			tag_meta_stcs-prot.h
			tag_meta-prot.h
			tag_meta_fnc.c
			tag_meta_stcs.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
