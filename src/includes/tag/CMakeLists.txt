target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			tag_basics.h
			tag_meta-defs.h
			tag_basics-defs.h
			tag_fldmap-defs.h
			tag_comfnc-defs.h
			tag_id3_helper-defs.h
			tag_id3v1.h
			tag_vorbc.h
			tag_id3v2-defs.h
			tag_vorbc-defs.h
			tag_id3v2.h
			tag_id3_helper.h
			tag_apev2-defs.h
			tag_apev2.h
			tag_fldmap.h
			tag_meta.h
			tag_conv-defs.h
			tag_conv.h
			tag_comfnc.h
			tag_id3v1-defs.h
	)
