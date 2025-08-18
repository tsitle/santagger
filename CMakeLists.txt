cmake_minimum_required(VERSION 3.23)  # due to 'target_sources(<target> xxx FILE_SET xxx)'
                                      # see https://cmake.org/cmake/help/latest/command/target_sources.html#file-sets
project(santagger_app_and_lib C)

if (MSVC)
	message(FATAL_ERROR "__ST__: MSVC Compiler is not supported")
endif ()

# ----------------------------------------------------------------------------

option(BUILD_FOR_VALGRIND "Build for Valgrind" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(STRIP_AFTER_BUILD "Strip executable after build" OFF)

option(INSTALL_PKG_CONFIG_MODULE "Install santagger.pc file" ON)

set(X_OPT 2)       # 0|1|2|3|s   (Optimization level (s=optimize for size))
set(X_PED "nor")   # nor|err|no  (be pedantic (warning level) ?)
set(X_WALL "yes")  # no|yes      (show warnings ?)

# Shared Library ABI version
set(X_LIB_ABI_VERSION 1)

# ----------------------------------------------------------------------------

set(CMAKE_C_STANDARD 17)

#
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

message(STATUS "__ST__: Build Type: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------------
# Handle options
# ----------------------------------------------------------------------------

set(OUTPUT_NAME_SUFFIX "")
set(Z_VAR_REQU_BUILD_STATIC OFF)

# Check if coverage flags are set
if (CMAKE_CXX_FLAGS MATCHES "(-fprofile-arcs|-ftest-coverage|--coverage|-fprofile-instr-generate|-fcoverage-mapping)")
	message(STATUS "__ST__: Building with coverage enabled '${CMAKE_CXX_FLAGS}' - requires static build")
	set(Z_VAR_REQU_BUILD_STATIC ON)
else ()
	message(STATUS "__ST__: Building without coverage")
endif ()

if (BUILD_FOR_VALGRIND)
	message(STATUS "__ST__: Building for Valgrind - requires static build")
	set(Z_VAR_REQU_BUILD_STATIC ON)
endif ()

if (Z_VAR_REQU_BUILD_STATIC AND BUILD_SHARED_LIBS)
	message(FATAL_ERROR "__ST__: Need to build static executables/library (BUILD_SHARED_LIBS=OFF)")
endif ()
if (BUILD_FOR_VALGRIND AND STRIP_AFTER_BUILD)
	message(FATAL_ERROR "__ST__: Cannot strip executables/library when building for Valgrind")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND STRIP_AFTER_BUILD)
	message(FATAL_ERROR "__ST__: Stripping executables/library with Build Type 'Debug' is nonsensical")
endif ()

#
set(Z_VAR_EXT_LIBRARY_LINK_TYPE "shared")
if (NOT BUILD_SHARED_LIBS)
	set(OUTPUT_NAME_SUFFIX "-static")
	# for external libraries:
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(Z_VAR_EXT_LIBRARY_LINK_TYPE "static")
endif ()

if (BUILD_FOR_VALGRIND)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(OUTPUT_NAME_SUFFIX "${OUTPUT_NAME_SUFFIX}-vg_debug")
	else ()
		set(OUTPUT_NAME_SUFFIX "${OUTPUT_NAME_SUFFIX}-vg_release")
	endif ()
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(OUTPUT_NAME_SUFFIX "${OUTPUT_NAME_SUFFIX}-debug")
endif ()

# ----------------------------------------------------------------------------

# add shared library headers
add_library(santagger_includes INTERFACE)
add_subdirectory("src/includes")

# add shared library
set(LIBRARY_ST_PUBLIC_NAME "santagger")

add_library(santagger_lib)
set_target_properties(santagger_lib PROPERTIES OUTPUT_NAME "${LIBRARY_ST_PUBLIC_NAME}${OUTPUT_NAME_SUFFIX}")
target_include_directories(santagger_lib PRIVATE "${PROJECT_SOURCE_DIR}")
add_subdirectory("src/lib")

# add executable
add_executable(santagger_app)
set_target_properties(santagger_app PROPERTIES OUTPUT_NAME "santagger")
add_subdirectory("src/app")
target_link_libraries(santagger_app PRIVATE santagger_lib santagger_includes)

# installation
include(GNUInstallDirs)
message(STATUS "__ST__: Install directories: PREFIX='${CMAKE_INSTALL_PREFIX}', BIN='${CMAKE_INSTALL_BINDIR}', LIB='${CMAKE_INSTALL_LIBDIR}', INC='${CMAKE_INSTALL_INCLUDEDIR}'")
install(TARGETS santagger_app santagger_lib
	RUNTIME
		COMPONENT Runtime
		DESTINATION ${CMAKE_INSTALL_BINDIR}  # Executables
	LIBRARY
		COMPONENT Runtime
		DESTINATION ${CMAKE_INSTALL_LIBDIR}  # Shared libraries
	ARCHIVE
		COMPONENT Runtime
		DESTINATION ${CMAKE_INSTALL_LIBDIR}  # Static libraries
)
install(DIRECTORY src/includes/
	COMPONENT Development
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/santagger
	FILES_MATCHING PATTERN "*.h"
)
set_target_properties(santagger_app PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib64")

# add executables for tests
add_executable(santagger_test_binobj)
add_executable(santagger_test_dl)
add_executable(santagger_test_m64)
add_executable(santagger_test_mtes)
add_executable(santagger_test_stream)
add_executable(santagger_test_strrd)
add_executable(santagger_test_strwr)
add_executable(santagger_test_sysfile)
add_executable(santagger_test_sysfnc)
add_executable(santagger_test_tfldmap)

add_subdirectory(src/tests)

# ----------------------------------------------------------------------------
# Strip executable
# ----------------------------------------------------------------------------

if (STRIP_AFTER_BUILD AND NOT BUILD_FOR_VALGRIND)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		message(STATUS "__ST__: Strip executable/lib")
		if (APPLE)
			set(TMP_TARGET_LIST santagger_app;santagger_lib)
			foreach(target ${TMP_TARGET_LIST})
				add_custom_command(TARGET ${target} POST_BUILD
					COMMAND strip -x -S $<TARGET_FILE:${target}>)
			endforeach ()
		else ()
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
			set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")
		endif ()
	else ()
		message(STATUS "__ST__: not stripping executable/lib (!release)")
	endif ()
elseif (STRIP_AFTER_BUILD AND BUILD_FOR_VALGRIND)
	message(STATUS "__ST__: not stripping executable/lib (valgrind)")
endif ()

# ----------------------------------------------------------------------------
# Compiler flags and some definitions
# ----------------------------------------------------------------------------

# Warnings
if (X_WALL STREQUAL "yes")
	message(STATUS "__ST__: Compiler flags: w/ Warnings")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Winline")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
else ()
	message(STATUS "__ST__: Compiler flags: w/o Warnings")
endif ()

# By default, symbols in shared libraries are "exported," meaning that they are visible to other code that links to
# the shared library. This can lead to symbol collisions and increased binary size if you export more symbols than necessary.
#
# The '-fvisibility=hidden' flag tells the compiler to make all symbols hidden by default,
# except those you explicitly mark as public. Hidden symbols are not exposed outside the shared library.
# This can improve performance and reduce the chances of symbol collisions by making only a few carefully
# chosen symbols available to external code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")

# Optimization Level
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "__ST__: Compiler flags: set OptLev=0 (debug)")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	message(STATUS "__ST__: Compiler flags: w/ implicit OptLev=x (release/relWithDebInfo/minSizeRel)")
else ()
	message(STATUS "__ST__: Compiler flags: set OptLev=${X_OPT} (other)")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O${X_OPT}")
endif ()

# Inline Functions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "__ST__: Compiler flags: w/o InlineFncs (debug)")
else ()
	if (X_OPT GREATER 0)
		if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
			message(STATUS "__ST__: Compiler flags: w/ InlineFncs (gcc)")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fkeep-inline-functions")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finline-functions")
			add_compile_definitions(ST_INLINE_S=inline)
		elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
			# Clang does not support the 'keep-inline-functions' flag
			message(STATUS "__ST__: Compiler flags: w/o InlineFncs (Clang)")
		else ()
			message(STATUS "__ST__: Compiler flags: w/o InlineFncs (unknown compiler '${CMAKE_C_COMPILER_ID}')")
		endif ()
	else ()
		message(STATUS "__ST__: Compiler flags: w/o InlineFncs (X_OPT=0)")
	endif ()
endif ()

# GDB
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "__ST__: Compiler flags: w/ Debug")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
	# enable debugging stuff
	add_compile_definitions(CONFIG_ST_ALL_DEBUG=1)
	# enable additional debugging stuff
	add_compile_definitions(CONFIG_ST_ALL_DEBUG_ADD=1)
else ()
	message(STATUS "__ST__: Compiler flags: w/o Debug")
endif ()

# Pedantic
if (X_PED STREQUAL "err")
	message(STATUS "__ST__: Compiler flags: w/ PedanticErrors")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic-errors")
else ()
	if (X_PED STREQUAL "nor")
		message(STATUS "__ST__: Compiler flags: w/ PedanticNormal")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
	else ()
		message(STATUS "__ST__: Compiler flags: w/o Pedantic")
	endif ()
endif ()

# Static
if (NOT BUILD_SHARED_LIBS)
	message(STATUS "__ST__: Compiler flags for lib: w/ Static")
	# for santagger_lib:
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	#
	if (NOT APPLE)
		message(STATUS "__ST__: Compiler flags for exe: w/ Static")
		# for the executable:
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
		#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
	else ()
		message(STATUS "__ST__: Compiler flags for exe: w/o Static")
	endif ()
else ()
	message(STATUS "__ST__: Compiler flags: w/o Static")
endif ()

#if (X_PROF STREQUAL "yes")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#endif ()

#if (X_USE_C STREQUAL "gcc")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
#endif ()

if (BUILD_FOR_VALGRIND)
	# add flags for Valgrind
	message(STATUS "__ST__: Compiler flags: Valgrind")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fno-inline -O0 -Wno-inline")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif ()

# ----------------------------------------------------------------------------
# Compiler definitions
# ----------------------------------------------------------------------------

# check for 64 bit host system
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "__ST__: Compiler defs: 64-bit host system detected (pointer size)")
	add_compile_definitions(_FILE_OFFSET_BITS=64)
	add_compile_definitions(_LARGEFILE_SOURCE)
	add_compile_definitions(CONFIG_ST_ALL_HAVE64BIT=1)
else ()
	message(STATUS "__ST__: Compiler defs: 32-bit host system detected (pointer size)")
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|aarch64|ppc64|mips64")
	message(STATUS "__ST__: Compiler defs: 64-bit host system detected (processor)")
	add_compile_definitions(CONFIG_ST_ALL_NATIVE64BIT=1)
else ()
	message(STATUS "__ST__: Compiler defs: 32-bit host system detected (processor)")
endif ()

# include the built-in TestBigEndian module
include(TestBigEndian)

# test for big-endian or little-endian
test_big_endian(IS_BIG_ENDIAN)

if (IS_BIG_ENDIAN)
	message(STATUS "__ST__: Compiler defs: Big-endian")
	add_compile_definitions(WORDS_BIGENDIAN=1)
else ()
	message(STATUS "__ST__: Compiler defs: Little-endian")
	add_compile_definitions(WORDS_BIGENDIAN=0)
endif ()

# include the CheckFunctionExists module
include(CheckFunctionExists)

# check if fseeko() exists
check_function_exists(fseeko HAVE_FSEEKO)
if (HAVE_FSEEKO)
	message(STATUS "__ST__: Compiler defs: HAVE_FSEEKO=1")
	add_compile_definitions(HAVE_FSEEKO=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_FSEEKO")
endif ()

# check if readlink() exists
check_function_exists(readlink HAVE_READLINK)
if (HAVE_READLINK)
	message(STATUS "__ST__: Compiler defs: HAVE_READLINK=1")
	add_compile_definitions(HAVE_READLINK=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_READLINK")
endif ()

# check if gettimeofday() exists
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
if (HAVE_GETTIMEOFDAY)
	message(STATUS "__ST__: Compiler defs: HAVE_GETTIMEOFDAY=1")
	add_compile_definitions(HAVE_GETTIMEOFDAY=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_GETTIMEOFDAY")
endif ()

# check if nanosleep() exists
check_function_exists(nanosleep HAVE_NANOSLEEP)
if (HAVE_NANOSLEEP)
	message(STATUS "__ST__: Compiler defs: HAVE_NANOSLEEP=1")
	add_compile_definitions(HAVE_NANOSLEEP=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_NANOSLEEP")
endif ()

# include the CheckStructHasMember module
include(CheckStructHasMember)

# check if 'struct timeval' has the member 'tv_sec'
check_struct_has_member("struct timeval" tv_sec sys/select.h HAVE_STRUCT_TIMEVAL_TV_SEC LANGUAGE C)
if (HAVE_STRUCT_TIMEVAL_TV_SEC)
	message(STATUS "__ST__: Compiler defs: HAVE_STRUCT_TIMEVAL_TV_SEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMEVAL_TV_SEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_STRUCT_TIMEVAL_TV_SEC")
endif ()
# check if 'struct timeval' has the member 'tv_usec'
check_struct_has_member("struct timeval" tv_usec sys/select.h HAVE_STRUCT_TIMEVAL_TV_USEC LANGUAGE C)
if (HAVE_STRUCT_TIMEVAL_TV_USEC)
	message(STATUS "__ST__: Compiler defs: HAVE_STRUCT_TIMEVAL_TV_USEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMEVAL_TV_USEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_STRUCT_TIMEVAL_TV_USEC")
endif ()

# check if 'struct timespec' has the member 'tv_sec'
check_struct_has_member("struct timespec" tv_sec sys/time.h HAVE_STRUCT_TIMESPEC_TV_SEC)
if (HAVE_STRUCT_TIMESPEC_TV_SEC)
	message(STATUS "__ST__: Compiler defs: HAVE_STRUCT_TIMESPEC_TV_SEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMESPEC_TV_SEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_STRUCT_TIMESPEC_TV_SEC")
endif ()
# check if 'struct timespec' has the member 'tv_nsec'
check_struct_has_member("struct timespec" tv_nsec sys/time.h HAVE_STRUCT_TIMESPEC_TV_NSEC)
if (HAVE_STRUCT_TIMESPEC_TV_NSEC)
	message(STATUS "__ST__: Compiler defs: HAVE_STRUCT_TIMESPEC_TV_NSEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMESPEC_TV_NSEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no HAVE_STRUCT_TIMESPEC_TV_NSEC")
endif ()

# ----------------------------------------------------------------------------
# Semantic version variables
# ----------------------------------------------------------------------------

set(X_FN_APP_VERSION_H "src/app/app_version.h")
set(X_FN_LIB_VERSION_H "src/includes/lib_version.h")

# Find Git
find_program(APP_GIT "git")
# Find Grep
find_program(APP_GREP "grep")

if (APP_GIT AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
	# get the current Git tag description (e.g. '1.0-2-ga12d3ef') for semantic versioning
	##
	execute_process(
			COMMAND ${APP_GIT} describe --tags --dirty
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_VERSION_SEM
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	## for testing the version parsing only:
	#set(GIT_VERSION_SEM "12.34-gdeadbeef")
	#set(GIT_VERSION_SEM "12.34-56-gdeadbeef")
	#set(GIT_VERSION_SEM "12.34-alpha5-gdeadbeef")
	#set(GIT_VERSION_SEM "12.34-alpha5-67-gdeadbeef")
	#set(GIT_VERSION_SEM "12.34-alpha5-67-gdeadbeef-dirty")
	#set(GIT_VERSION_SEM "1.0-alpha-3-g4fdbbae-dirty")
	#set(GIT_VERSION_SEM "1.0-alpha-g4fdbbae-dirty")
	#set(GIT_VERSION_SEM "1.0-alpha-g4fdbbae")
	#set(GIT_VERSION_SEM "1.0-g4fdbbae-dirty")
	#set(GIT_VERSION_SEM "1.0-g4fdbbae")
	#set(GIT_VERSION_SEM "1.0-beta")
	#set(GIT_VERSION_SEM "1.0")
	##
	if (GIT_VERSION_SEM MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Semantic version")
	endif ()

	# get only the major and minor version
	#   --> extract everything before the first dash (-)
	string(REGEX REPLACE "-.*" "" GIT_VERSION_MAJMIN "${GIT_VERSION_SEM}")
	#message(STATUS "Git version: mm '${GIT_VERSION_MAJMIN}'")
	if (GIT_VERSION_MAJMIN MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Major.Minor version")
	endif ()

	# get only the major version
	#   --> extract everything before the first dot (.)
	string(REGEX REPLACE "\\.[0-9]*" "" GIT_VERSION_MAJOR "${GIT_VERSION_MAJMIN}")
	if (GIT_VERSION_MAJOR MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Major version")
	endif ()

	# get only the minor version
	#   --> extract everything after the first dot (.)
	string(REGEX REPLACE "^([^\\.]*)\\.([^\\.]*)" "\\2" GIT_VERSION_MINOR "${GIT_VERSION_MAJMIN}")
	if (GIT_VERSION_MINOR MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Minor version")
	endif ()

	# get only the patch level
	#   --> in the example '1.2-ga12d3ef' that would be '0'
	#   --> in the example '1.2-3-ga12d3ef' that would be '3'
	#   --> in the example '1.2-alpha3-4-ga12d3ef' that would be 'alpha3-4'
	#   --> in the example '1.2-alpha3-4-ga12d3ef-dirty' that would be 'alpha3-4'
	string(REGEX REPLACE "-dirty$" "" GIT_VERSION_TMP "${GIT_VERSION_SEM}")
	#message(STATUS "Git version: tmp '${GIT_VERSION_TMP}'")
	string(REGEX REPLACE "-g[a-z0-9]*$" "" GIT_VERSION_TMP "${GIT_VERSION_TMP}")
	#message(STATUS "Git version: tmp '${GIT_VERSION_TMP}'")
	string(REGEX REPLACE "^${GIT_VERSION_MAJMIN}-" "" GIT_VERSION_TMP "${GIT_VERSION_TMP}")  # '12.34-5' --> '5'
	string(REGEX REPLACE "^${GIT_VERSION_MAJMIN}$" "0" GIT_VERSION_TMP "${GIT_VERSION_TMP}")  # '12.34' --> '0'
	#message(STATUS "Git version: tmp '${GIT_VERSION_TMP}'")

	string(REGEX REPLACE "^.*-([0-9]*)$" "\\1" GIT_VERSION_PATCH "${GIT_VERSION_TMP}")
	#message(STATUS "Git version: patch '${GIT_VERSION_PATCH}'")
	string(REGEX REPLACE "^alpha.*-" "" GIT_VERSION_PATCH "${GIT_VERSION_PATCH}")  # 'alpha-3' --> '3'
	string(REGEX REPLACE "^beta.*-" "" GIT_VERSION_PATCH "${GIT_VERSION_PATCH}")  # 'beta-3' --> '3'
	string(REGEX REPLACE "^alpha.*$" "0" GIT_VERSION_PATCH "${GIT_VERSION_PATCH}")  # 'alpha' --> '0'
	string(REGEX REPLACE "^beta.*$" "0" GIT_VERSION_PATCH "${GIT_VERSION_PATCH}")  # 'beta' --> '0'
	#message(STATUS "Git version: patch '${GIT_VERSION_PATCH}'")

	# get only the alpha/beta marker
	#   --> in the example '1.2-ga12d3ef' that would be ''
	#   --> in the example '1.2-3-ga12d3ef' that would be ''
	#   --> in the example '1.2-alpha3-4-ga12d3ef' that would be 'alpha3'
	#   --> in the example '1.2-alpha3-4-ga12d3ef-dirty' that would be 'alpha3'
	string(REGEX REPLACE "^[0-9]*\\.[0-9]*-(.*)$" "\\1" GIT_VERSION_ALPHABETA "${GIT_VERSION_TMP}")
	#message(STATUS "Git version: ab '${GIT_VERSION_ALPHABETA}'")

	string(REGEX REPLACE "^(.*)-[0-9]*$" "\\1" GIT_VERSION_ALPHABETA "${GIT_VERSION_ALPHABETA}")
	#message(STATUS "Git version: ab '${GIT_VERSION_ALPHABETA}'")
	string(REGEX REPLACE "^${GIT_VERSION_PATCH}$" "" GIT_VERSION_ALPHABETA "${GIT_VERSION_ALPHABETA}")
	#message(STATUS "Git version: ab '${GIT_VERSION_ALPHABETA}'")

	# --------------------------
	# get only the commit ID
	execute_process(
			COMMAND ${APP_GIT} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_VERSION_CID
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	if (GIT_VERSION_CID MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Commit ID")
	endif ()
elseif (EXISTS "${CMAKE_SOURCE_DIR}/${X_FN_LIB_VERSION_H}")
	message(STATUS "__ST__: Using existing lib_version.h")
	if (APP_GREP)
		set(Z_VAR_CMD_GREP "${APP_GREP}")
	elseif (WIN32)
		# we are in a MS Windows Terminal (and not in an UCRT64 environment)
		set(Z_VAR_CMD_GREP "findstr")
		set(X_FN_LIB_VERSION_H "src\\includes\\lib_version.h")
	else ()
		message(FATAL_ERROR "__ST__: No application found for 'grep'")
	endif ()
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_STRING " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_SEM WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_MAJOR " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_MAJOR WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_MINOR " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_MINOR WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_PATCH " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_PATCH WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_ALPHABETA " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_ALPHABETA WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${Z_VAR_CMD_GREP} " ST_LIBSANTAG_VERS_COMMITID " "${X_FN_LIB_VERSION_H}" OUTPUT_VARIABLE GIT_VERSION_CID WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)

	# --> extract everything in between the parenthesis - but without the quotation marks
	string(REGEX REPLACE "(.*)\\(\"(.*)\"\\)(.*)" "\\2" GIT_VERSION_SEM "${GIT_VERSION_SEM}")
	if (GIT_VERSION_SEM MATCHES "^$")
		message(FATAL_ERROR "__ST__: could not determine Git Semantic version")
	endif ()
	# --> extract everything in between the parenthesis
	string(REGEX REPLACE "(.*)\\((.*)\\)(.*)" "\\2" GIT_VERSION_MAJOR "${GIT_VERSION_MAJOR}")
	# --> extract everything in between the parenthesis
	string(REGEX REPLACE "(.*)\\((.*)\\)(.*)" "\\2" GIT_VERSION_MINOR "${GIT_VERSION_MINOR}")
	# --> extract everything in between the parenthesis
	string(REGEX REPLACE "(.*)\\((.*)\\)(.*)" "\\2" GIT_VERSION_PATCH "${GIT_VERSION_PATCH}")
	# --> extract everything in between the parenthesis - but without the quotation marks
	string(REGEX REPLACE "(.*)\\(\"(.*)\"\\)(.*)" "\\2" GIT_VERSION_ALPHABETA "${GIT_VERSION_ALPHABETA}")
	# --> extract everything in between the parenthesis - but without the quotation marks
	string(REGEX REPLACE "(.*)\\(\"(.*)\"\\)(.*)" "\\2" GIT_VERSION_CID "${GIT_VERSION_CID}")
endif ()

# ----------------------------------------------------------------------------
# Replace version strings and numbers in app_version.h and lib_version.h
# ----------------------------------------------------------------------------

# Find Sed
find_program(APP_SED "sed")

if (APP_SED AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
	message(STATUS "__ST__: Rewrite file '${X_FN_APP_VERSION_H}'")
	execute_process(
			INPUT_FILE "${X_FN_APP_VERSION_H}.in"
			COMMAND ${APP_SED} -e "s;###VERS_STRING###;${GIT_VERSION_SEM};g"
			COMMAND ${APP_SED} -e "s;###VERS_MAJOR###;${GIT_VERSION_MAJOR};g"
			COMMAND ${APP_SED} -e "s;###VERS_MINOR###;${GIT_VERSION_MINOR};g"
			COMMAND ${APP_SED} -e "s;###VERS_PATCH###;${GIT_VERSION_PATCH};g"
			COMMAND ${APP_SED} -e "s;###VERS_ALPHABETA###;${GIT_VERSION_ALPHABETA};g"
			COMMAND ${APP_SED} -e "s;###VERS_COMMITID###;${GIT_VERSION_CID};g"
			COMMAND ${APP_SED} -e "s;these will to be replaced by CMake;these have been replaced by CMake;g"
			OUTPUT_FILE "${X_FN_APP_VERSION_H}"
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)

	message(STATUS "__ST__: Rewrite file '${X_FN_LIB_VERSION_H}'")
	execute_process(
			INPUT_FILE "${X_FN_LIB_VERSION_H}.in"
			COMMAND ${APP_SED} -e "s;###VERS_STRING###;${GIT_VERSION_SEM};g"
			COMMAND ${APP_SED} -e "s;###VERS_MAJOR###;${GIT_VERSION_MAJOR};g"
			COMMAND ${APP_SED} -e "s;###VERS_MINOR###;${GIT_VERSION_MINOR};g"
			COMMAND ${APP_SED} -e "s;###VERS_PATCH###;${GIT_VERSION_PATCH};g"
			COMMAND ${APP_SED} -e "s;###VERS_ALPHABETA###;${GIT_VERSION_ALPHABETA};g"
			COMMAND ${APP_SED} -e "s;###VERS_COMMITID###;${GIT_VERSION_CID};g"
			COMMAND ${APP_SED} -e "s;these will to be replaced by CMake;these have been replaced by CMake;g"
			OUTPUT_FILE "${X_FN_LIB_VERSION_H}"
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
else ()
	message(STATUS "__ST__: Not rewriting files '${X_FN_APP_VERSION_H}' and '${X_FN_LIB_VERSION_H}'")
endif ()

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/${X_FN_APP_VERSION_H}")
	message(FATAL_ERROR "__ST__: Missing app_version.h")
endif ()
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/${X_FN_LIB_VERSION_H}")
	message(FATAL_ERROR "__ST__: Missing lib_version.h")
endif ()

message(STATUS "__ST__: Git version: Semantic=${GIT_VERSION_SEM}; Major=${GIT_VERSION_MAJOR}; Minor=${GIT_VERSION_MINOR}; A/B=${GIT_VERSION_ALPHABETA}; Patch=${GIT_VERSION_PATCH}; CID=${GIT_VERSION_CID}")

target_sources(santagger_app
	PRIVATE
		${X_FN_APP_VERSION_H}
)
target_sources(santagger_includes
	INTERFACE FILE_SET HEADERS
	BASE_DIRS ${PROJECT_SOURCE_DIR}
	FILES
		${X_FN_LIB_VERSION_H}
)

# ----------------------------------------------------------------------------
# Set version of Shared Library
# ----------------------------------------------------------------------------

set_target_properties(santagger_lib PROPERTIES
	VERSION ${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_ALPHABETA}${GIT_VERSION_PATCH}  # Full version
	SOVERSION ${X_LIB_ABI_VERSION}  # ABI version
)

# ----------------------------------------------------------------------------
# External Shared Libraries
# ----------------------------------------------------------------------------

# check for optional shared library 'libmpg123'
find_library(LIBMPG123 NAMES mpg123)
find_path(LIBMPG123_INCLUDE_DIR NAMES mpg123.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

set(Z_VAR_PC_REQU_MPG123 OFF)
set(Z_VAR_PC_REQU_MAD OFF)
if (LIBMPG123 AND LIBMPG123_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libmpg123: ${LIBMPG123}")
	add_compile_definitions(HAVE_LIBMPG123=1)

	if (NOT BUILD_SHARED_LIBS AND WIN32)
		set(MPG123_USE_STATIC_LIBS ON CACHE BOOL "Link mpg123 statically" FORCE)
	endif ()
	target_link_libraries(santagger_app PRIVATE ${LIBMPG123})
	include_directories(${LIBMPG123_INCLUDE_DIR})

	set(Z_VAR_PC_REQU_MPG123 ON)

	# libshlwapi is only required for MS Windows when mpg123 is being statically linked
	if (MINGW AND NOT BUILD_SHARED_LIBS)
		find_library(LIBSHLWAPI NAMES shlwapi)
		find_path(LIBSHLWAPI_INCLUDE_DIR NAMES shlwapi.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

		if (LIBSHLWAPI AND LIBSHLWAPI_INCLUDE_DIR)
			message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libshlwapi: ${LIBSHLWAPI}")

			target_link_libraries(santagger_app PRIVATE ${LIBSHLWAPI})
			include_directories(${LIBSHLWAPI_INCLUDE_DIR})
		else ()
			message(FATAL_ERROR "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libshlwapi not found")
		endif ()
	endif ()
else ()
	message(STATUS "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libmpg123 not found, trying libmad")

	find_library(LIBMAD NAMES mad)
	find_path(LIBMAD_INCLUDE_DIR NAMES mad.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

	if (LIBMAD AND LIBMAD_INCLUDE_DIR)
		message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libmad: ${LIBMAD}")
		add_compile_definitions(HAVE_LIBMAD=1)

		target_link_libraries(santagger_app PRIVATE ${LIBMAD})
		include_directories(${LIBMAD_INCLUDE_DIR})

		set(Z_VAR_PC_REQU_MAD ON)
	else ()
		message(STATUS "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libmad not found, proceeding without it")
	endif ()
endif ()

# check for optional shared libraries 'libvorbis' and 'libogg'
find_library(LIBVORBIS NAMES vorbis)
find_path(LIBVORBIS_INCLUDE_DIR NAMES vorbis/codec.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

find_library(LIBOGG NAMES ogg)
find_path(LIBOGG_INCLUDE_DIR NAMES ogg/ogg.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

set(Z_VAR_NEED_LIBM OFF)
set(Z_VAR_PC_REQU_VORBIS OFF)
set(Z_VAR_PC_REQU_OGG OFF)
if (LIBVORBIS AND LIBVORBIS_INCLUDE_DIR AND LIBOGG AND LIBOGG_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libvorbis: ${LIBVORBIS}")
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libogg: ${LIBOGG}")
	add_compile_definitions(HAVE_LIBVORBIS=1)

	target_link_libraries(santagger_app PRIVATE ${LIBVORBIS} ${LIBOGG})
	include_directories(${LIBVORBIS_INCLUDE_DIR} ${LIBOGG_INCLUDE_DIR})

	set(Z_VAR_PC_REQU_VORBIS ON)
	set(Z_VAR_PC_REQU_OGG ON)
	if (NOT APPLE AND NOT WIN32)
		set(Z_VAR_NEED_LIBM ON)
	endif ()
else ()
	message(STATUS "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libvorbis+libogg not found, proceeding without it")
endif ()

# check for optional shared library 'zlib'
set(Z_VAR_ZLIB_STATIC_EXTRA_NAME "")
if (MINGW AND NOT BUILD_SHARED_LIBS)
	set(Z_VAR_ZLIB_STATIC_EXTRA_NAME "zlibstatic")
elseif (MINGW)
	set(Z_VAR_ZLIB_STATIC_EXTRA_NAME "zlib")
endif ()
find_library(LIBZ NAMES z ${Z_VAR_ZLIB_STATIC_EXTRA_NAME} PATHS /usr/local/opt/zlib/lib)
find_path(LIBZ_INCLUDE_DIR NAMES zlib.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

set(Z_VAR_PC_REQU_LIBZ OFF)
if (LIBZ AND LIBZ_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libz: ${LIBZ}")
	add_compile_definitions(HAVE_LIBZ=1)

	target_link_libraries(santagger_app PRIVATE ${LIBZ})
	include_directories(${LIBZ_INCLUDE_DIR})

	set(Z_VAR_PC_REQU_LIBZ ON)
else ()
	message(STATUS "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libz not found, proceeding without it")
endif ()

# libbcrypt
set(Z_VAR_PC_REQU_BCRYPT OFF)
if (WIN32)
	# link to shared library 'libbcrypt' - required only for MS Windows
	find_library(LIBBCRYPT NAMES bcrypt)
	if (LIBBCRYPT)
		message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libbcrypt: ${LIBBCRYPT}")
	else ()
		message(FATAL_ERROR "__ST__: libbcrypt not found")
	endif ()
	target_link_libraries(santagger_lib PRIVATE ${LIBBCRYPT})

	set(Z_VAR_PC_REQU_BCRYPT ON)
endif ()

# libm
set(Z_VAR_PC_REQU_LIBM OFF)
if (Z_VAR_NEED_LIBM)
	# link to shared library 'libm' - required only for Debian when using libvorbis
	find_library(LIBM NAMES m)
	if (LIBM)
		message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libm: ${LIBM}")
		target_link_libraries(santagger_app PRIVATE ${LIBM})

		set(Z_VAR_PC_REQU_LIBM ON)
	else ()
		message(STATUS "__ST__: libm not found")
	endif ()
endif ()

# ----------------------------------------------------------------------------
# Pkg-config
# ----------------------------------------------------------------------------

# Helper function to configure pkg-config files
function(configure_pkg_config_file pkg_config_file_in)
	set(prefix ${CMAKE_INSTALL_PREFIX})
	set(exec_prefix ${CMAKE_INSTALL_FULL_BINDIR})
	set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
	set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
	set(VERSION ${GIT_VERSION_SEM})
	set(REQU_PRIV "")
	if (BUILD_SHARED_LIBS)
		if (Z_VAR_PC_REQU_MPG123)
			set(REQU_PRIV "${REQU_PRIV} libmpg123")
		endif ()
		if (Z_VAR_PC_REQU_MAD)
			set(REQU_PRIV "${REQU_PRIV} mad")
		endif ()
		if (Z_VAR_PC_REQU_VORBIS)
			set(REQU_PRIV "${REQU_PRIV} vorbis")
		endif ()
		if (Z_VAR_PC_REQU_OGG)
			set(REQU_PRIV "${REQU_PRIV} ogg")
		endif ()
		if (Z_VAR_PC_REQU_LIBZ)
			set(REQU_PRIV "${REQU_PRIV} zlib")
		endif ()
	endif ()
	set(LIBS_PRIV "")
	if (BUILD_SHARED_LIBS)
		if (Z_VAR_PC_REQU_BCRYPT)
			set(LIBS_PRIV "${LIBS_PRIV} -lbcrypt")
		endif ()
		if (Z_VAR_PC_REQU_LIBM)
			set(LIBS_PRIV "${LIBS_PRIV} -lm")
		endif ()
	endif ()
	string(REPLACE ".in" "" pkg_config_file ${pkg_config_file_in})
	configure_file(${pkg_config_file_in} ${pkg_config_file} @ONLY)
endfunction()

if (INSTALL_PKG_CONFIG_MODULE)
	configure_pkg_config_file(santagger.pc.in)

	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/santagger.pc
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	)
endif ()
