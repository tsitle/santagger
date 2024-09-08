target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			cont_ogg.h
			cont_wav.h
			audvid.h
			aud_flac-defs.h
			aud_mpeg1-defs.h
			cont_ogg-defs.h
			cont_wav-defs.h
			cont_ogg_vorbcomm.h
			audvid-defs.h
			aud_mpeg1.h
	)

add_subdirectory("ogg_sub")
