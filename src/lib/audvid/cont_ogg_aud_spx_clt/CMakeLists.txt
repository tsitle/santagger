target_sources(santagger_lib
		PRIVATE
			ac_spx_clt_zrd.c
			ac_spx_clt-prot.h
			ac_spx_clt_stcs-prot.h
			ac_spx_clt_stcs.c
			ac_spx_clt_zrd-prot.h
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
