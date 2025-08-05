target_sources(santagger_tests_binobj
	PRIVATE
		test_binobj.c
)
target_link_libraries(santagger_tests_binobj PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_dl
	PRIVATE
		test_dl.c
)
target_link_libraries(santagger_tests_dl PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_m64
	PRIVATE
		test_m64.c
)
target_link_libraries(santagger_tests_m64 PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_mtes
	PRIVATE
		test_mtes.c
)
target_link_libraries(santagger_tests_mtes PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_rnd
	PRIVATE
		test_rnd.c
)
target_link_libraries(santagger_tests_rnd PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_stream
	PRIVATE
		test_stream.c
)
target_link_libraries(santagger_tests_stream PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_strrd
	PRIVATE
		test_strrd.c
)
target_link_libraries(santagger_tests_strrd PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_strwr
	PRIVATE
		test_strwr.c
)
target_link_libraries(santagger_tests_strwr PRIVATE santagger_lib santagger_includes)

target_sources(santagger_tests_tfldmap
	PRIVATE
		test_tfldmap.c
)
target_link_libraries(santagger_tests_tfldmap PRIVATE santagger_lib santagger_includes)
