cmake_minimum_required(VERSION 3.23)  # due to 'target_sources(<target> xxx FILE_SET xxx)'
                                      # see https://cmake.org/cmake/help/latest/command/target_sources.html#file-sets
project(santagger_app_and_lib C)

# ----------------------------------------------------------------------------

set(X_OPT 2)        # 0|1|2|3|s   (Optimization level (s=optimize for size))
set(X_STATIC "no")  # no|yes      (create static or shared library ?)
set(X_PED "nor")    # nor|err|no  (be pedantic (warning level) ?)
set(X_WALL "yes")   # no|yes      (show warnings ?)

# Shared Library ABI version
set(X_LIB_AVI_VERSION 1)

# ----------------------------------------------------------------------------

set(CMAKE_C_STANDARD 17)

#
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------------

# add shared library headers
add_library(santagger_includes INTERFACE)
add_subdirectory("src/includes")

# add shared library
add_library(santagger_lib SHARED)
set_target_properties(santagger_lib PROPERTIES OUTPUT_NAME "santagger")
target_include_directories(santagger_lib PRIVATE "${PROJECT_SOURCE_DIR}")
add_subdirectory("src/lib")

# add executable
add_executable(santagger_app)
set_target_properties(santagger_app PROPERTIES OUTPUT_NAME "santagger")
add_subdirectory("src/app")
target_link_libraries(santagger_app PRIVATE santagger_lib santagger_includes)

# installation
include(GNUInstallDirs)
message(STATUS "Install directories: BIN='${CMAKE_INSTALL_BINDIR}', LIB='${CMAKE_INSTALL_LIBDIR}', INC='${CMAKE_INSTALL_INCLUDEDIR}'")
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
		FILES_MATCHING PATTERN "*.h")

# ----------------------------------------------------------------------------
# Compiler flags and some definitions
# ----------------------------------------------------------------------------

# Warnings
if (X_WALL STREQUAL "yes")
	message(STATUS "Compiler flags: w/ Warnings")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Winline")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
else ()
	message(STATUS "Compiler flags: w/o Warnings")
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
	message(STATUS "Compiler flags: set OptLev=0 (debug)")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	message(STATUS "Compiler flags: w/ implicit OptLev=x (release/relWithDebInfo/minSizeRel)")
else ()
	message(STATUS "Compiler flags: set OptLev=${X_OPT} (other)")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O${X_OPT}")
endif ()

# Inline Functions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Compiler flags: w/o InlineFncs (debug)")
else ()
	if (X_OPT GREATER 0)
		if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
			message(STATUS "Compiler flags: w/ InlineFncs (gcc)")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fkeep-inline-functions")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finline-functions")
			add_compile_definitions(ST_INLINE_S=inline)
		elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
			# Clang does not support the 'keep-inline-functions' flag
			message(STATUS "Compiler flags: w/o InlineFncs (Clang)")
		else ()
			message(STATUS "Compiler flags: w/o InlineFncs (unknown compiler '${CMAKE_C_COMPILER_ID}')")
		endif ()
	else ()
		message(STATUS "Compiler flags: w/o InlineFncs (X_OPT=0)")
	endif ()
endif ()

# GDB
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Compiler flags: w/ Debug")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
	# enable debugging stuff
	add_compile_definitions(CONFIG_ST_ALL_DEBUG=1)
	# enable additional debugging stuff
	add_compile_definitions(CONFIG_ST_ALL_DEBUG_ADD=1)
else ()
	message(STATUS "Compiler flags: w/o Debug")
endif ()

# Pedantic
if (X_PED STREQUAL "err")
	message(STATUS "Compiler flags: w/ PedanticErrors")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic-errors")
else ()
	if (X_PED STREQUAL "nor")
		message(STATUS "Compiler flags: w/ PedanticNormal")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
	else ()
		message(STATUS "Compiler flags: w/o Pedantic")
	endif ()
endif ()

# Static
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	message(STATUS "Compiler flags: w/ implicit Static (release)")
else ()
	if (X_STATIC STREQUAL "yes")
		message(STATUS "Compiler flags: w/ Static (!release)")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	else ()
		message(STATUS "Compiler flags: w/o Static (!release)")
	endif ()
endif ()

#if (X_PROF STREQUAL "yes")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#endif ()

#if (X_USE_C STREQUAL "gcc")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
#endif ()

# ----------------------------------------------------------------------------
# Compiler definitions
# ----------------------------------------------------------------------------

# check for 64 bit host system
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "Compiler defs: 64-bit host system detected (pointer size)")
	add_compile_definitions(_FILE_OFFSET_BITS=64)
	add_compile_definitions(_LARGEFILE_SOURCE)
	add_compile_definitions(CONFIG_ST_ALL_HAVE64BIT=1)
else ()
	message(STATUS "Compiler defs: 32-bit host system detected (pointer size)")
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|aarch64|ppc64|mips64")
	message(STATUS "Compiler defs: 64-bit host system detected (processor)")
	add_compile_definitions(CONFIG_ST_ALL_NATIVE64BIT=1)
else ()
	message(STATUS "Compiler defs: 32-bit host system detected (processor)")
endif ()

# include the built-in TestBigEndian module
include(TestBigEndian)

# test for big-endian or little-endian
test_big_endian(IS_BIG_ENDIAN)

if (IS_BIG_ENDIAN)
	message(STATUS "Compiler defs: Big-endian")
	add_compile_definitions(WORDS_BIGENDIAN=1)
else ()
	message(STATUS "Compiler defs: Little-endian")
	add_compile_definitions(WORDS_BIGENDIAN=0)
endif ()

# include the CheckFunctionExists module
include(CheckFunctionExists)

# check if fseeko() exists
check_function_exists(fseeko HAVE_FSEEKO)
if (HAVE_FSEEKO)
	message(STATUS "Compiler defs: HAVE_FSEEKO=1")
	add_compile_definitions(HAVE_FSEEKO=1)
else ()
	message(STATUS "Compiler defs: no HAVE_FSEEKO")
endif ()

# check if readlink() exists
check_function_exists(readlink HAVE_READLINK)
if (HAVE_READLINK)
	message(STATUS "Compiler defs: HAVE_READLINK=1")
	add_compile_definitions(HAVE_READLINK=1)
else ()
	message(STATUS "Compiler defs: no HAVE_READLINK")
endif ()

# check if gettimeofday() exists
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
if (HAVE_GETTIMEOFDAY)
	message(STATUS "Compiler defs: HAVE_GETTIMEOFDAY=1")
	add_compile_definitions(HAVE_GETTIMEOFDAY=1)
else ()
	message(STATUS "Compiler defs: no HAVE_GETTIMEOFDAY")
endif ()

# include the CheckStructHasMember module
include(CheckStructHasMember)

# check if 'struct timeval' has the member 'tv_sec'
check_struct_has_member("struct timeval" tv_sec sys/select.h HAVE_STRUCT_TIMEVAL_TV_SEC LANGUAGE C)
if (HAVE_STRUCT_TIMEVAL_TV_SEC)
	message(STATUS "Compiler defs: HAVE_STRUCT_TIMEVAL_TV_SEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMEVAL_TV_SEC=1)
else ()
	message(STATUS "Compiler defs: no HAVE_STRUCT_TIMEVAL_TV_SEC")
endif ()
# check if 'struct timeval' has the member 'tv_usec'
check_struct_has_member("struct timeval" tv_usec sys/select.h HAVE_STRUCT_TIMEVAL_TV_USEC LANGUAGE C)
if (HAVE_STRUCT_TIMEVAL_TV_USEC)
	message(STATUS "Compiler defs: HAVE_STRUCT_TIMEVAL_TV_USEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMEVAL_TV_USEC=1)
else ()
	message(STATUS "Compiler defs: no HAVE_STRUCT_TIMEVAL_TV_USEC")
endif ()

# check if 'struct timespec' has the member 'tv_sec'
check_struct_has_member("struct timespec" tv_sec sys/time.h HAVE_STRUCT_TIMESPEC_TV_SEC)
if (HAVE_STRUCT_TIMESPEC_TV_SEC)
	message(STATUS "Compiler defs: HAVE_STRUCT_TIMESPEC_TV_SEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMESPEC_TV_SEC=1)
else ()
	message(STATUS "Compiler defs: no HAVE_STRUCT_TIMESPEC_TV_SEC")
endif ()
# check if 'struct timespec' has the member 'tv_nsec'
check_struct_has_member("struct timespec" tv_nsec sys/time.h HAVE_STRUCT_TIMESPEC_TV_NSEC)
if (HAVE_STRUCT_TIMESPEC_TV_NSEC)
	message(STATUS "Compiler defs: HAVE_STRUCT_TIMESPEC_TV_NSEC=1")
	add_compile_definitions(HAVE_STRUCT_TIMESPEC_TV_NSEC=1)
else ()
	message(STATUS "Compiler defs: no HAVE_STRUCT_TIMESPEC_TV_NSEC")
endif ()

# ----------------------------------------------------------------------------
# Semantic version variables
# ----------------------------------------------------------------------------

# get the current Git tag description (e.g. '1.0-2-ga12d3ef') for semantic versioning
execute_process(
		COMMAND git describe --tags --dirty
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_VERSION_SEM
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
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

# get only the major and minor version
#   --> extract everything before the first dash (-)
string(REGEX REPLACE "-.*" "" GIT_VERSION_MAJMIN "${GIT_VERSION_SEM}")
#message(STATUS "Git version: mm '${GIT_VERSION_MAJMIN}'")

# get only the major version
#   --> extract everything before the first dot (.)
string(REGEX REPLACE "\\.[0-9]*" "" GIT_VERSION_MAJOR "${GIT_VERSION_MAJMIN}")

# get only the minor version
#   --> extract everything after the first dot (.)
string(REGEX REPLACE "^([^\\.]*)\\.([^\\.]*)" "\\2" GIT_VERSION_MINOR "${GIT_VERSION_MAJMIN}")

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

# ----------------------------------------------------------------------------
# Replace version strings and numbers in app_version.h and lib_version.h
# ----------------------------------------------------------------------------

message(STATUS "Git version: Semantic=${GIT_VERSION_SEM}; Major=${GIT_VERSION_MAJOR}; Minor=${GIT_VERSION_MINOR}; A/B=${GIT_VERSION_ALPHABETA}; Patch=${GIT_VERSION_PATCH}")

set(X_FN_APP_VERSION_H "src/app/app_version.h")
message(STATUS "Rewrite file '${X_FN_APP_VERSION_H}'")
execute_process(
		INPUT_FILE "${X_FN_APP_VERSION_H}.in"
		COMMAND sed -e "s;###VERS_STRING###;${GIT_VERSION_SEM};g"
		COMMAND sed -e "s;###VERS_MAJOR###;${GIT_VERSION_MAJOR};g"
		COMMAND sed -e "s;###VERS_MINOR###;${GIT_VERSION_MINOR};g"
		COMMAND sed -e "s;###VERS_PATCH###;${GIT_VERSION_PATCH};g"
		COMMAND sed -e "s;###VERS_ALPHABETA###;${GIT_VERSION_ALPHABETA};g"
		COMMAND sed -e "s;these will to be replaced by CMake;these have been replaced by CMake;g"
		OUTPUT_FILE "${X_FN_APP_VERSION_H}"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)

set(X_FN_LIB_VERSION_H "src/includes/lib_version.h")
message(STATUS "Rewrite file '${X_FN_LIB_VERSION_H}'")
execute_process(
		INPUT_FILE "${X_FN_LIB_VERSION_H}.in"
		COMMAND sed -e "s;###VERS_STRING###;${GIT_VERSION_SEM};g"
		COMMAND sed -e "s;###VERS_MAJOR###;${GIT_VERSION_MAJOR};g"
		COMMAND sed -e "s;###VERS_MINOR###;${GIT_VERSION_MINOR};g"
		COMMAND sed -e "s;###VERS_PATCH###;${GIT_VERSION_PATCH};g"
		COMMAND sed -e "s;###VERS_ALPHABETA###;${GIT_VERSION_ALPHABETA};g"
		COMMAND sed -e "s;these will to be replaced by CMake;these have been replaced by CMake;g"
		OUTPUT_FILE "${X_FN_LIB_VERSION_H}"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)

# ----------------------------------------------------------------------------
# Set version of Shared Library
# ----------------------------------------------------------------------------

set_target_properties(santagger_lib PROPERTIES
		VERSION ${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_ALPHABETA}${GIT_VERSION_PATCH}  # Full version
		SOVERSION ${X_LIB_AVI_VERSION}  # ABI version
	)

# ----------------------------------------------------------------------------
# External Shared Libraries
# ----------------------------------------------------------------------------

# check for optional shared library 'libmad'
find_library(LIBMAD NAMES mad)
find_path(LIBMAD_INCLUDE_DIR NAMES mad.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

if (LIBMAD AND LIBMAD_INCLUDE_DIR)
	message(STATUS "Found libmad: ${LIBMAD}")
	add_compile_definitions(HAVE_LIBMAD=1)

	target_link_libraries(santagger_app PRIVATE ${LIBMAD})
	include_directories(${LIBMAD_INCLUDE_DIR})
else ()
	message(STATUS "libmad not found, trying libmpg123")

	# check for optional shared library 'libmpg123'
	find_library(LIBMPG123 NAMES mpg123)
	find_path(LIBMPG123_INCLUDE_DIR NAMES mpg123.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

	if (LIBMPG123 AND LIBMPG123_INCLUDE_DIR)
		message(STATUS "Found libmpg123: ${LIBMPG123}")
		add_compile_definitions(HAVE_LIBMPG123=1)

		target_link_libraries(santagger_app PRIVATE ${LIBMPG123})
		include_directories(${LIBMPG123_INCLUDE_DIR})
	else ()
		message(STATUS "libmpg123 not found, proceeding without it")
	endif ()
endif ()

# check for optional shared libraries 'libvorbis' and 'libogg'
find_library(LIBVORBIS NAMES vorbis)
find_path(LIBVORBIS_INCLUDE_DIR NAMES vorbis/codec.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

find_library(LIBOGG NAMES ogg)
find_path(LIBOGG_INCLUDE_DIR NAMES ogg/ogg.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

if (LIBVORBIS AND LIBVORBIS_INCLUDE_DIR AND LIBOGG AND LIBOGG_INCLUDE_DIR)
	message(STATUS "Found libvorbis: ${LIBVORBIS}")
	message(STATUS "Found libogg: ${LIBOGG}")
	add_compile_definitions(HAVE_LIBVORBIS=1)

	target_link_libraries(santagger_app PRIVATE ${LIBVORBIS} ${LIBOGG})
	include_directories(${LIBVORBIS_INCLUDE_DIR} ${LIBOGG_INCLUDE_DIR})
else ()
	message(STATUS "libvorbis+libogg not found, proceeding without it")
endif ()

# check for optional shared library 'libz'
find_library(LIBZ NAMES z)
find_path(LIBZ_INCLUDE_DIR NAMES zlib.h PATHS /usr/include /usr/local/include /opt/include /opt/local/include)

if (LIBZ AND LIBZ_INCLUDE_DIR)
	message(STATUS "Found libz: ${LIBZ}")
	add_compile_definitions(HAVE_LIBZ=1)

	target_link_libraries(santagger_app PRIVATE ${LIBZ})
	include_directories(${LIBZ_INCLUDE_DIR})
else ()
	message(STATUS "libz not found, proceeding without it")
endif ()
