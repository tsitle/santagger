target_sources(santagger_app
		PRIVATE
			app_defs.h
			main.c
			app_version.h
	)

add_subdirectory("mf")
add_subdirectory("cln")
