target_sources(santagger_lib
		PRIVATE
			mc_vorbcomm_zrd-prot.h
			mc_vorbcomm_stcs.c
			mc_vorbcomm-prot.h
			mc_vorbcomm_gs-prot.h
			mc_vorbcomm_gs.c
			mc_vorbcomm_zrd.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
