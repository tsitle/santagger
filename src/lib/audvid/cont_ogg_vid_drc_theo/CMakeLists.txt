target_sources(santagger_lib
		PRIVATE
			vc_drc_theo_stcs.c
			vc_drc_theo-prot.h
			vc_drc_theo_zrd.c
			vc_drc_theo_zrd-prot.h
			vc_drc_theo_stcs-prot.h
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
