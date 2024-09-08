target_sources(santagger_lib
		PRIVATE
			mc_skel_zstcs-prot.h
			mc_skel_zrd.c
			mc_skel_zstcs.c
			mc_skel-prot.h
			mc_skel_zrd-prot.h
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
