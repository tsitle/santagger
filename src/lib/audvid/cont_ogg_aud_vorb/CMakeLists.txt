target_sources(santagger_lib
		PRIVATE
			ac_vorb_zrd-prot.h
			ac_vorb_stcs.c
			ac_vorb_stcs-prot.h
			ac_vorb_zrd.c
			ac_vorb-prot.h
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
