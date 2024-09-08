target_sources(santagger_lib
		PRIVATE
			ac_flac_zrda-pp.h
			ac_flac_stcs-prot.h
			ac_flac-prot.h
			ac_flac_zrdh-pp.h
			ac_flac_stcs.c
			ac_flac_zrdh.c
			ac_flac_zrd.c
			ac_flac_zrd-prot.h
			ac_flac_zrda.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
