target_sources(santagger_lib
		PRIVATE
			cont_wav_zfdeb-prot.h
			cont_wav_stcs-prot.h
			cont_wav_stcs.c
			cont_wav-prot.h
			cont_wav_zfnc-prot.h
			cont_wav_gs.c
			cont_wav_opts.c
			cont_wav_zfdeb.c
			cont_wav_zfnc.c
			cont_wav_wr.c
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
	)
