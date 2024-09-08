target_sources(santagger_lib
		PRIVATE
			dynlist_stcs-prot.h
			dynlist-priv.h
			dynlist.c
			dynlist-prot.h
			dynlist_stcs.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
