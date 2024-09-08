target_sources(santagger_lib
		PRIVATE
			tag_apev2_gs.c
			tag_apev2.c
			tag_apev2_zwr.c
			tag_apev2_zrd.c
			tag_apev2_zrd-pp.h
			tag_apev2-prot.h
			tag_apev2_ztidy.c
			tag_apev2_stcs.c
			tag_apev2_zfdeb-prot.h
			tag_apev2_fldadd.c
			tag_apev2_opts.c
			tag_apev2_zwr-pp.h
			tag_apev2_fnc.c
			tag_apev2_zfdeb.c
			tag_apev2_fnc-prot.h
			tag_apev2_fldget.c
			tag_apev2_stcs-prot.h
			tag_apev2_gs-prot.h
			tag_apev2_ztidy-prot.h
		PUBLIC FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			tag_apev2-priv_flds.h
	)
