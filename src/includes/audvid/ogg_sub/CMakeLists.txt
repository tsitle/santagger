target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			ac_spx_clt-defs.h
			ac_vorb-defs.h
			ac_flac-defs.h
			mc_vorbcomm-defs.h
			vc_drc_theo-defs.h
	)
