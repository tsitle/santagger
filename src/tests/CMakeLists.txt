target_sources(santagger_test_binobj
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_binobj.c
)
target_link_libraries(santagger_test_binobj PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_dl
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_dl.c
)
target_link_libraries(santagger_test_dl PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_m64
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_m64.c
)
target_link_libraries(santagger_test_m64 PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_mtes
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_mtes.c
)
target_link_libraries(santagger_test_mtes PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_stream
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_stream.c
)
target_link_libraries(santagger_test_stream PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_strrd
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_strrd.c
)
target_link_libraries(santagger_test_strrd PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_strwr
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_strwr.c
)
target_link_libraries(santagger_test_strwr PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_sysfile
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_sysfile.c
)
target_link_libraries(santagger_test_sysfile PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_sysfnc
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_sysfnc.c
)
target_link_libraries(santagger_test_sysfnc PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_tfldmap
	PRIVATE
		fncs_test_common.c
		fncs_test_common.h
		test_tfldmap.c
)
target_link_libraries(santagger_test_tfldmap PRIVATE santagger_lib santagger_includes)
