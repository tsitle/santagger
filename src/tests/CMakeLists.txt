target_sources(santagger_test_binobj
	PRIVATE
		test_binobj.c
)
target_link_libraries(santagger_test_binobj PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_dl
	PRIVATE
		test_dl.c
)
target_link_libraries(santagger_test_dl PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_m64
	PRIVATE
		test_m64.c
)
target_link_libraries(santagger_test_m64 PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_mtes
	PRIVATE
		test_mtes.c
)
target_link_libraries(santagger_test_mtes PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_stream
	PRIVATE
		test_stream.c
)
target_link_libraries(santagger_test_stream PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_strrd
	PRIVATE
		test_strrd.c
)
target_link_libraries(santagger_test_strrd PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_strwr
	PRIVATE
		test_strwr.c
)
target_link_libraries(santagger_test_strwr PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_sysfile
	PRIVATE
		test_sysfile.c
)
target_link_libraries(santagger_test_sysfile PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_sysfnc
	PRIVATE
		test_sysfnc.c
)
target_link_libraries(santagger_test_sysfnc PRIVATE santagger_lib santagger_includes)

target_sources(santagger_test_tfldmap
	PRIVATE
		test_tfldmap.c
)
target_link_libraries(santagger_test_tfldmap PRIVATE santagger_lib santagger_includes)
