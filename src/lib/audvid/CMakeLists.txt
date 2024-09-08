target_sources(santagger_lib
		PRIVATE
			av_zfdeb-prot.h
			av_zfdeb.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)

add_subdirectory("cont_ogg_aud_spx_clt")
add_subdirectory("cont_ogg_vid_drc_theo")
add_subdirectory("cont_ogg_aud_flac")
add_subdirectory("aud_mpeg1")
add_subdirectory("cont_ogg_aud_vorb")
add_subdirectory("cont_ogg_met_vorbcomm")
add_subdirectory("cont_ogg_met_skel")
add_subdirectory("cont_wav")
add_subdirectory("cont_ogg")
