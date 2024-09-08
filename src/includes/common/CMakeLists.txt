target_sources(santagger_includes
		INTERFACE FILE_SET HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES
			sys_file-defs.h
			sys_math.h
			dynlist-defs.h
			string_mte-defs.h
			string_mte.h
			streamrd-defs.h
			dynlist.h
			binobj.h
			binobj-defs.h
			streamwr-defs.h
			sys_fnc.h
			streamrd.h
			streamwr.h
			sys_math-defs.h
			sys_file.h
	)
