#
# by TS, Sep 2025
#

function (ts_strip_targets_after_build MSG_PREFIX TARGETS_TO_STRIP_LIST)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		if (APPLE)
			foreach(target ${TARGETS_TO_STRIP_LIST})
				message(STATUS "${MSG_PREFIX}: Enable Stripping for target '${target}' (macOS)")
				add_custom_command(TARGET ${target} POST_BUILD
						COMMAND strip -x -S $<TARGET_FILE:${target}>)
			endforeach ()
		else ()
			foreach(target ${TARGETS_TO_STRIP_LIST})
				message(STATUS "${MSG_PREFIX}: Enable Stripping for target '${target}' (!macOS)")
			endforeach ()
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")
		endif ()
	else ()
		foreach(target ${TARGETS_TO_STRIP_LIST})
			message(STATUS "${MSG_PREFIX}: not enabling Stripping for target '${target}' (!release)")
		endforeach ()
	endif ()
endfunction ()

# ----------------------------------------------------------------------------------------------------------------------

function (ts_find_line_in_file FILE_INP PATTERN OUTP_VAR)
	set(${OUTP_VAR} "" PARENT_SCOPE)
	file(STRINGS ${FILE_INP} TMP_ARR_LINES)

	foreach (TMP_LINE IN LISTS TMP_ARR_LINES)
		string(REGEX MATCH ${PATTERN} TMP_PMATCH "${TMP_LINE}")
		if (NOT TMP_PMATCH MATCHES "^$")
			set(${OUTP_VAR} "${TMP_PMATCH}" PARENT_SCOPE)
			break()
		endif ()
	endforeach ()
endfunction ()

# ----------------------------------------------------------------------------------------------------------------------

function (ts_read_git_version_from_c_header_file MSG_PREFIX FILE_INP C_DEFINE_NAME OUTP_VAR IS_STRING)
	if (NOT EXISTS "${FILE_INP}")
		message(FATAL_ERROR "${MSG_PREFIX}: file '${FILE_INP}' not found")
	endif ()
	ts_find_line_in_file("${FILE_INP}" "^#define ${C_DEFINE_NAME} .*" TMP_GIT_VERSION)
	if (TMP_GIT_VERSION MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not read version from file '${FILE_INP}' (C_DEFINE_NAME='${C_DEFINE_NAME}')")
	endif ()

	if (IS_STRING)
		# --> extract everything in between the parenthesis - but without the quotation marks
		string(REGEX REPLACE "(.*)\\(\"(.*)\"\\)(.*)" "\\2" TMP_GIT_VERSION "${TMP_GIT_VERSION}")
	else ()
		string(REGEX REPLACE "(.*)\\((.*)\\)(.*)" "\\2" TMP_GIT_VERSION "${TMP_GIT_VERSION}")
	endif ()

	set(${OUTP_VAR} "${TMP_GIT_VERSION}" PARENT_SCOPE)
	#message(STATUS "${MSG_PREFIX}: FILE_INP='${FILE_INP}' C_DEFINE_NAME='${C_DEFINE_NAME}' OUTP_VAR='${TMP_GIT_VERSION}'")
endfunction ()

function (ts_output_git_version_from_c_header_file MSG_PREFIX FILE_INP C_DEFINE_NAME APP_OR_LIB_NAME IS_STRING)
	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME}" TMP_OUTP_VERS IS_STRING)

	message(STATUS "${MSG_PREFIX}: ${APP_OR_LIB_NAME} version='${TMP_OUTP_VERS}'")
endfunction ()

# ----------------------------------------------------------------------------------------------------------------------

function (ts_git__check_version MSG_PREFIX VERS_GIT_SEM VERS_GIT_MAJ VERS_GIT_MIN VERS_GIT_PAT VERS_GIT_AB VERS_GIT_CID)
	if (VERS_GIT_SEM MATCHES "^$" OR VERS_GIT_SEM MATCHES ".*#define .*")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Semantic version")
	endif ()
	if (VERS_GIT_MAJ MATCHES "^$" OR VERS_GIT_MAJ MATCHES ".*#define .*")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Major version")
	endif ()
	if (VERS_GIT_MIN MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Minor version")
	endif ()
	if (VERS_GIT_PAT MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Patch version")
	endif ()
	if (VERS_GIT_CID MATCHES "^$" OR VERS_GIT_CID MATCHES ".*#define .*")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Commit ID")
	endif ()
endfunction ()

function (ts_git_read_version__gitdir MSG_PREFIX GIT_DIRECTORY FILE_INP OUTP_VAR_GIT_SEM OUTP_VAR_GIT_MAJ OUTP_VAR_GIT_MIN OUTP_VAR_GIT_PAT OUTP_VAR_GIT_AB OUTP_VAR_GIT_CID)
	find_program(APP_GIT "git")

	# get the current Git tag description (e.g. '1.0-2-ga12d3ef') for semantic versioning
	##
	execute_process(
			COMMAND ${APP_GIT} describe --tags --dirty
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE TMP_GIT_VERSION_SEM
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	## for testing the version parsing only:
	#set(TMP_GIT_VERSION_SEM "12.34-gdeadbeef")
	#set(TMP_GIT_VERSION_SEM "12.34-56-gdeadbeef")
	#set(TMP_GIT_VERSION_SEM "12.34-alpha5-gdeadbeef")
	#set(TMP_GIT_VERSION_SEM "12.34-alpha5-67-gdeadbeef")
	#set(TMP_GIT_VERSION_SEM "12.34-alpha5-67-gdeadbeef-dirty")
	#set(TMP_GIT_VERSION_SEM "1.0-alpha-3-g4fdbbae-dirty")
	#set(TMP_GIT_VERSION_SEM "1.0-alpha-g4fdbbae-dirty")
	#set(TMP_GIT_VERSION_SEM "1.0-alpha-g4fdbbae")
	#set(TMP_GIT_VERSION_SEM "1.0-g4fdbbae-dirty")
	#set(TMP_GIT_VERSION_SEM "1.0-g4fdbbae")
	#set(TMP_GIT_VERSION_SEM "1.0-beta")
	#set(TMP_GIT_VERSION_SEM "1.0")
	##
	if (TMP_GIT_VERSION_SEM MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Semantic version")
	endif ()
	set(${OUTP_VAR_GIT_SEM} "${TMP_GIT_VERSION_SEM}" PARENT_SCOPE)

	# get only the major and minor version
	#   --> extract everything before the first dash (-)
	string(REGEX REPLACE "-.*" "" TMP_GIT_VERSION_MAJMIN "${TMP_GIT_VERSION_SEM}")
	#message(STATUS "Git version: mm '${TMP_GIT_VERSION_MAJMIN}'")

	# get only the major version
	#   --> extract everything before the first dot (.)
	string(REGEX REPLACE "\\.[0-9]*" "" TMP_GIT_VERSION_MAJOR "${TMP_GIT_VERSION_MAJMIN}")
	set(${OUTP_VAR_GIT_MAJ} "${TMP_GIT_VERSION_MAJOR}" PARENT_SCOPE)

	# get only the minor version
	#   --> extract everything after the first dot (.)
	string(REGEX REPLACE "^([^\\.]*)\\.([^\\.]*)" "\\2" TMP_GIT_VERSION_MINOR "${TMP_GIT_VERSION_MAJMIN}")
	set(${OUTP_VAR_GIT_MIN} "${TMP_GIT_VERSION_MINOR}" PARENT_SCOPE)

	# get only the patch level
	#   --> in the example '1.2-ga12d3ef' that would be '0'
	#   --> in the example '1.2-3-ga12d3ef' that would be '3'
	#   --> in the example '1.2-alpha3-4-ga12d3ef' that would be 'alpha3-4'
	#   --> in the example '1.2-alpha3-4-ga12d3ef-dirty' that would be 'alpha3-4'
	string(REGEX REPLACE "-dirty$" "" TMP_GIT_VERSION_XXX "${TMP_GIT_VERSION_SEM}")
	#message(STATUS "Git version: tmp '${TMP_GIT_VERSION_XXX}'")
	string(REGEX REPLACE "-g[a-z0-9]*$" "" TMP_GIT_VERSION_XXX "${TMP_GIT_VERSION_XXX}")
	#message(STATUS "Git version: tmp '${TMP_GIT_VERSION_XXX}'")
	string(REGEX REPLACE "^${TMP_GIT_VERSION_MAJMIN}-" "" TMP_GIT_VERSION_XXX "${TMP_GIT_VERSION_XXX}")  # '12.34-5' --> '5'
	string(REGEX REPLACE "^${TMP_GIT_VERSION_MAJMIN}$" "0" TMP_GIT_VERSION_XXX "${TMP_GIT_VERSION_XXX}")  # '12.34' --> '0'
	#message(STATUS "Git version: tmp '${TMP_GIT_VERSION_XXX}'")

	string(REGEX REPLACE "^.*-([0-9]*)$" "\\1" TMP_GIT_VERSION_PATCH "${TMP_GIT_VERSION_XXX}")
	#message(STATUS "Git version: patch '${TMP_GIT_VERSION_PATCH}'")
	string(REGEX REPLACE "^alpha.*-" "" TMP_GIT_VERSION_PATCH "${TMP_GIT_VERSION_PATCH}")  # 'alpha-3' --> '3'
	string(REGEX REPLACE "^beta.*-" "" TMP_GIT_VERSION_PATCH "${TMP_GIT_VERSION_PATCH}")  # 'beta-3' --> '3'
	string(REGEX REPLACE "^alpha.*$" "0" TMP_GIT_VERSION_PATCH "${TMP_GIT_VERSION_PATCH}")  # 'alpha' --> '0'
	string(REGEX REPLACE "^beta.*$" "0" TMP_GIT_VERSION_PATCH "${TMP_GIT_VERSION_PATCH}")  # 'beta' --> '0'
	#message(STATUS "Git version: patch '${TMP_GIT_VERSION_PATCH}'")
	set(${OUTP_VAR_GIT_PAT} "${TMP_GIT_VERSION_PATCH}" PARENT_SCOPE)

	# get only the alpha/beta marker
	#   --> in the example '1.2-ga12d3ef' that would be ''
	#   --> in the example '1.2-3-ga12d3ef' that would be ''
	#   --> in the example '1.2-alpha3-4-ga12d3ef' that would be 'alpha3'
	#   --> in the example '1.2-alpha3-4-ga12d3ef-dirty' that would be 'alpha3'
	string(REGEX REPLACE "^[0-9]*\\.[0-9]*-(.*)$" "\\1" TMP_GIT_VERSION_ALPHABETA "${TMP_GIT_VERSION_XXX}")
	#message(STATUS "Git version: ab '${TMP_GIT_VERSION_ALPHABETA}'")

	string(REGEX REPLACE "^(.*)-[0-9]*$" "\\1" TMP_GIT_VERSION_ALPHABETA "${TMP_GIT_VERSION_ALPHABETA}")
	#message(STATUS "Git version: ab '${TMP_GIT_VERSION_ALPHABETA}'")
	string(REGEX REPLACE "^${TMP_GIT_VERSION_PATCH}$" "" TMP_GIT_VERSION_ALPHABETA "${TMP_GIT_VERSION_ALPHABETA}")
	#message(STATUS "Git version: ab '${TMP_GIT_VERSION_ALPHABETA}'")
	set(${OUTP_VAR_GIT_AB} "${TMP_GIT_VERSION_ALPHABETA}" PARENT_SCOPE)

	# --------------------------
	# get only the commit ID
	execute_process(
			COMMAND ${APP_GIT} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE TMP_GIT_VERSION_CID
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if (TMP_GIT_VERSION_CID MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Commit ID")
	endif ()
	set(${OUTP_VAR_GIT_CID} "${TMP_GIT_VERSION_CID}" PARENT_SCOPE)
endfunction ()

function (ts_git_read_version__file MSG_PREFIX GIT_DIRECTORY FILE_INP C_DEFINE_NAME_PREFIX OUTP_VAR_GIT_SEM OUTP_VAR_GIT_MAJ OUTP_VAR_GIT_MIN OUTP_VAR_GIT_PAT OUTP_VAR_GIT_AB OUTP_VAR_GIT_CID)
	message(STATUS "${MSG_PREFIX}: Using existing '${FILE_INP}'")

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_STRING" TMP_VERS TRUE)
	set(${OUTP_VAR_GIT_SEM} "${TMP_VERS}" PARENT_SCOPE)
	if (TMP_VERS MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Semantic version")
	endif ()

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_MAJOR" TMP_VERS FALSE)
	set(${OUTP_VAR_GIT_MAJ} "${TMP_VERS}" PARENT_SCOPE)

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_MINOR" TMP_VERS FALSE)
	set(${OUTP_VAR_GIT_MIN} "${TMP_VERS}" PARENT_SCOPE)

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_PATCH" TMP_VERS FALSE)
	set(${OUTP_VAR_GIT_PAT} "${TMP_VERS}" PARENT_SCOPE)

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_ALPHABETA" TMP_VERS TRUE)
	set(${OUTP_VAR_GIT_AB} "${TMP_VERS}" PARENT_SCOPE)

	ts_read_git_version_from_c_header_file("${MSG_PREFIX}" "${FILE_INP}" "${C_DEFINE_NAME_PREFIX}_VERS_COMMITID" TMP_VERS TRUE)
	set(${OUTP_VAR_GIT_CID} "${TMP_VERS}" PARENT_SCOPE)
endfunction ()

function (ts_git_read_version MSG_PREFIX GIT_DIRECTORY FILE_INP C_DEFINE_NAME_PREFIX OUTP_VAR_GIT_SEM OUTP_VAR_GIT_MAJ OUTP_VAR_GIT_MIN OUTP_VAR_GIT_PAT OUTP_VAR_GIT_AB OUTP_VAR_GIT_CID)
	find_program(APP_GIT "git")

	if (APP_GIT AND EXISTS "${GIT_DIRECTORY}")
		ts_git_read_version__gitdir(
				"${MSG_PREFIX}"
				"${GIT_DIRECTORY}"
				"${FILE_INP}"
				TMP_VERS_GIT_SEM
				TMP_VERS_GIT_MAJ
				TMP_VERS_GIT_MIN
				TMP_VERS_GIT_PAT
				TMP_VERS_GIT_AB
				TMP_VERS_GIT_CID
		)
	elseif (EXISTS "${FILE_INP}")
		ts_git_read_version__file(
				"${MSG_PREFIX}"
				"${GIT_DIRECTORY}"
				"${FILE_INP}"
				"${C_DEFINE_NAME_PREFIX}"
				TMP_VERS_GIT_SEM
				TMP_VERS_GIT_MAJ
				TMP_VERS_GIT_MIN
				TMP_VERS_GIT_PAT
				TMP_VERS_GIT_AB
				TMP_VERS_GIT_CID
		)
	else ()
		message(FATAL_ERROR "${MSG_PREFIX}: could not determine Git Semantic version - neither Git dir nor version file available")
	endif ()
	set(${OUTP_VAR_GIT_SEM} "${TMP_VERS_GIT_SEM}" PARENT_SCOPE)
	set(${OUTP_VAR_GIT_MAJ} "${TMP_VERS_GIT_MAJ}" PARENT_SCOPE)
	set(${OUTP_VAR_GIT_MIN} "${TMP_VERS_GIT_MIN}" PARENT_SCOPE)
	set(${OUTP_VAR_GIT_PAT} "${TMP_VERS_GIT_PAT}" PARENT_SCOPE)
	set(${OUTP_VAR_GIT_AB} "${TMP_VERS_GIT_AB}" PARENT_SCOPE)
	set(${OUTP_VAR_GIT_CID} "${TMP_VERS_GIT_CID}" PARENT_SCOPE)
endfunction ()

function (ts_git__write_version_file MSG_PREFIX GIT_DIRECTORY TEMPLATE_FILE_INP FILE_OUTP VERS_GIT_SEM VERS_GIT_MAJ VERS_GIT_MIN VERS_GIT_PAT VERS_GIT_AB VERS_GIT_CID)
	ts_git__check_version(MSG_PREFIX "${VERS_GIT_SEM}" "${VERS_GIT_MAJ}" "${VERS_GIT_MIN}" "${VERS_GIT_PAT}" "${VERS_GIT_AB}" "${VERS_GIT_CID}")

	message(STATUS "${MSG_PREFIX}: Git version: Semantic=${VERS_GIT_SEM}; Major=${VERS_GIT_MAJ}; Minor=${VERS_GIT_MIN}; A/B=${VERS_GIT_AB}; Patch=${VERS_GIT_PAT}; CID=${VERS_GIT_CID}")

	if (EXISTS "${FILE_OUTP}.locked")
		message(STATUS "${MSG_PREFIX}: not rewriting file '${FILE_OUTP}' (locked)")
		return()
	endif ()

	file(TOUCH "${FILE_OUTP}.locked")

	if (EXISTS "${GIT_DIRECTORY}")
		string(RANDOM LENGTH 16 TMP_RND_STR)
		message(STATUS "${MSG_PREFIX}: Rewriting file '${FILE_OUTP}' (#${TMP_RND_STR})")

		file(STRINGS ${TEMPLATE_FILE_INP} TMP_ARR_LINES)

		file(WRITE "${FILE_OUTP}-${TMP_RND_STR}" "")
		foreach (TMP_LINE IN LISTS TMP_ARR_LINES)
			string(REPLACE "these will be replaced by CMake" "these have been replaced by CMake" TMP_PMATCH "${TMP_LINE}")
			string(REPLACE "###VERS_STRING###" "${VERS_GIT_SEM}" TMP_PMATCH "${TMP_PMATCH}")
			string(REPLACE "###VERS_MAJOR###" "${VERS_GIT_MAJ}" TMP_PMATCH "${TMP_PMATCH}")
			string(REPLACE "###VERS_MINOR###" "${VERS_GIT_MIN}" TMP_PMATCH "${TMP_PMATCH}")
			string(REPLACE "###VERS_PATCH###" "${VERS_GIT_PAT}" TMP_PMATCH "${TMP_PMATCH}")
			string(REPLACE "###VERS_ALPHABETA###" "${VERS_GIT_AB}" TMP_PMATCH "${TMP_PMATCH}")
			string(REPLACE "###VERS_COMMITID###" "${VERS_GIT_CID}" TMP_PMATCH "${TMP_PMATCH}")
			file(APPEND "${FILE_OUTP}-${TMP_RND_STR}" "${TMP_PMATCH}\n")
		endforeach ()

		file(RENAME "${FILE_OUTP}-${TMP_RND_STR}" "${FILE_OUTP}")
	else ()
		message(STATUS "${MSG_PREFIX}: not rewriting file '${FILE_OUTP}' (no Git dir)")
	endif ()

	file(REMOVE "${FILE_OUTP}.locked")

	if (NOT EXISTS "${FILE_OUTP}")
		message(FATAL_ERROR "${MSG_PREFIX}: Missing ${FILE_OUTP}")
	endif ()
endfunction ()

function (ts_git_read_version_and_write_to_file MSG_PREFIX GIT_DIRECTORY TEMPLATE_FILE_INP FILE_OUTP C_DEFINE_NAME_PREFIX)
	ts_git_read_version(
			"${MSG_PREFIX}"
			"${GIT_DIRECTORY}"
			"${FILE_OUTP}"
			"${C_DEFINE_NAME_PREFIX}"
			OUTP_VAR_GIT_SEM
			OUTP_VAR_GIT_MAJ
			OUTP_VAR_GIT_MIN
			OUTP_VAR_GIT_PAT
			OUTP_VAR_GIT_AB
			OUTP_VAR_GIT_CID
	)

	ts_git__write_version_file(
			"${MSG_PREFIX}"
			"${GIT_DIRECTORY}"
			"${TEMPLATE_FILE_INP}"
			"${FILE_OUTP}"
			"${OUTP_VAR_GIT_SEM}"
			"${OUTP_VAR_GIT_MAJ}"
			"${OUTP_VAR_GIT_MIN}"
			"${OUTP_VAR_GIT_PAT}"
			"${OUTP_VAR_GIT_AB}"
			"${OUTP_VAR_GIT_CID}"
	)
endfunction ()

# ----------------------------------------------------------------------------------------------------------------------

# Helper function to configure pkg-config files
function(ts_pkgconfig__configure_pkg_config_file PKG_CONFIG_TEMPLATE_FILE_DIR PKG_CONFIG_TEMPLATE_FILE_INP PKG_CONFIG_OUTP_FILE_SUFFIX GIT_VERS_SEM PC_REQU_PRIV PC_LIBS_PRIV)
	if (GIT_VERS_SEM MATCHES "^$")
		message(FATAL_ERROR "${MSG_PREFIX}: empty version - cannot configure pkg-config file")
	endif ()

	set(prefix "${CMAKE_INSTALL_PREFIX}")
	set(exec_prefix "${CMAKE_INSTALL_FULL_BINDIR}")
	set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
	set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
	set(VERSION "${GIT_VERS_SEM}")
	set(REQU_PRIV "${PC_REQU_PRIV}")
	set(LIBS_PRIV "${PC_LIBS_PRIV}")
	string(REPLACE ".pc.in" "${PKG_CONFIG_OUTP_FILE_SUFFIX}.pc" TMP_OUTP_FILE "${PKG_CONFIG_TEMPLATE_FILE_INP}")
	configure_file("${PKG_CONFIG_TEMPLATE_FILE_DIR}/${PKG_CONFIG_TEMPLATE_FILE_INP}" "${TMP_OUTP_FILE}" @ONLY)
endfunction()
