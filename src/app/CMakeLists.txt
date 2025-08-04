target_sources(santagger_app
		PRIVATE
			app_defs.h
			main.c
	)

add_subdirectory("mf")
add_subdirectory("cln")
