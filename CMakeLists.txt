cmake_minimum_required(VERSION 3.23)  # due to 'target_sources(<target> xxx FILE_SET xxx)'
                                      # see https://cmake.org/cmake/help/latest/command/target_sources.html#file-sets
project(santagger_app_and_lib_project C)

set(CMAKE_C_STANDARD 17)

if (MSVC)
	message(FATAL_ERROR "__ST__: MSVC Compiler is not supported")
endif ()

# ----------------------------------------------------------------------------
# CMake Modules
# ----------------------------------------------------------------------------

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-mods" ${CMAKE_MODULE_PATH})

include(TsUtils RESULT_VARIABLE Z_INC_UTILS)
if (NOT Z_INC_UTILS)
	message(FATAL_ERROR "__ST__: could not include TsUtils")
endif ()

# ----------------------------------------------------------------------------
# CMake Options
# ----------------------------------------------------------------------------

option(BUILD_FOR_VALGRIND "Build for Valgrind" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(STRIP_AFTER_BUILD "Strip executable after build" OFF)

option(INSTALL_PKG_CONFIG_MODULE "Install santagger.pc file" ON)

# ----------------------------------------------------------------------------

set(X_OPT 2)       # 0|1|2|3|s   (Optimization level (s=optimize for size))
set(X_PED "nor")   # nor|err|no  (be pedantic (warning level) ?)
set(X_WALL "yes")  # no|yes      (show warnings ?)

# Shared Library ABI version
set(X_LIB_ABI_VERSION 1)

# ----------------------------------------------------------------------------

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
# Add targets
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

if (STRIP_AFTER_BUILD)
	set(TMP_TARGET_LIST santagger_app;santagger_lib)  # semi-colon separated list
	if (NOT BUILD_FOR_VALGRIND)
		ts_strip_targets_after_build("__ST__" "${TMP_TARGET_LIST}")
	else ()
		message(STATUS "__ST__: not enabling Stripping for targets '${TMP_TARGET_LIST}' (valgrind)")
	endif ()
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
set(Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_H "")  # this isn't currently set
set(Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "__ST__: Compiler flags: w/o InlineFncs (debug)")
else ()
	if (X_OPT GREATER 0)
		if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
			message(STATUS "__ST__: Compiler flags: w/ InlineFncs (gcc)")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fkeep-inline-functions")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finline-functions")
			set(Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S "inline")
			add_compile_definitions(LIBSANTAGGER_KWFNC_INLINE_S=${Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S})
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
if (Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_H MATCHES "^$")
	add_compile_definitions(LIBSANTAGGER_KWFNC_INLINE_H=)
endif ()
if (Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S MATCHES "^$")
	add_compile_definitions(LIBSANTAGGER_KWFNC_INLINE_S=)
endif ()

# GDB
set(Z_COMPDEFS_LIBSANTAGGER_CFG_DEBUG OFF)
set(Z_COMPDEFS_LIBSANTAGGER_CFG_EXTRA_DEBUG OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "__ST__: Compiler flags: w/ Debug")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
	# enable debugging stuff
	add_compile_definitions(LIBSANTAGGER_CFG_DEBUG=1)
	set(Z_COMPDEFS_LIBSANTAGGER_CFG_DEBUG ON)
	# enable additional debugging stuff
	add_compile_definitions(LIBSANTAGGER_CFG_EXTRA_DEBUG=1)
	set(Z_COMPDEFS_LIBSANTAGGER_CFG_EXTRA_DEBUG ON)
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
set(Z_COMPDEFS_LIBSANTAGGER_PPM_FILE_OFFSET_BITS OFF)
set(Z_COMPDEFS_LIBSANTAGGER_PPM_LARGEFILE_SOURCE OFF)
set(Z_COMPDEFS_LIBSANTAGGER_CFG_HAVE64BIT OFF)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "__ST__: Compiler defs: 64-bit host system detected (pointer size)")
	#
	# When '_FILE_OFFSET_BITS' is defined as 64, it enables support for handling files larger than 2GB on 32-bit systems.
	# The macro must be defined before including any header files to ensure all file-related functions use 64-bit offsets.
	# When enabled, it affects several aspects of file handling:
	#   - Makes `off_t` and related types 64-bit
	#   - Ensures file operations (like `fseek`, `lseek`, etc.) use 64-bit variants
	#   - Modifies structures like `struct stat` to use 64-bit fields
	add_compile_definitions(_FILE_OFFSET_BITS=64)
	set(Z_COMPDEFS_LIBSANTAGGER_PPM_FILE_OFFSET_BITS ON)
	#
	# '_LARGEFILE_SOURCE' part of the LFS (Large File Support) extension:
	#    - Enables support for handling large files (>2GB) on 32-bit systems
	#    - Often used in conjunction with `_FILE_OFFSET_BITS=64`
	# When defined, it makes available the basic set of large file interfaces.
	# It must be defined before including any system headers.
	# Historical Context:
	#   - Was introduced to maintain binary compatibility with older applications while providing new interfaces for large file support
	#   - Part of the POSIX standard's approach to handling files larger than 2GB on 32-bit systems
	add_compile_definitions(_LARGEFILE_SOURCE=1)
	set(Z_COMPDEFS_LIBSANTAGGER_PPM_LARGEFILE_SOURCE ON)
	#
	add_compile_definitions(LIBSANTAGGER_CFG_HAVE64BIT=1)
	set(Z_COMPDEFS_LIBSANTAGGER_CFG_HAVE64BIT ON)
else ()
	message(STATUS "__ST__: Compiler defs: 32-bit host system detected (pointer size)")
endif ()

set(Z_COMPDEFS_LIBSANTAGGER_CFG_NATIVE64BIT OFF)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|aarch64|ppc64|mips64")
	message(STATUS "__ST__: Compiler defs: 64-bit host system detected (processor)")
	add_compile_definitions(LIBSANTAGGER_CFG_NATIVE64BIT=1)
	set(Z_COMPDEFS_LIBSANTAGGER_CFG_NATIVE64BIT ON)
else ()
	message(STATUS "__ST__: Compiler defs: 32-bit host system detected (processor)")
endif ()

# include the built-in TestBigEndian module
include(TestBigEndian)

# test for big-endian or little-endian
test_big_endian(IS_BIG_ENDIAN)

set(Z_COMPDEFS_LIBSANTAGGER_CFG_WORDS_BIGENDIAN OFF)
if (IS_BIG_ENDIAN)
	message(STATUS "__ST__: Compiler defs: Big-endian")
	add_compile_definitions(LIBSANTAGGER_CFG_WORDS_BIGENDIAN=1)
	set(Z_COMPDEFS_LIBSANTAGGER_CFG_WORDS_BIGENDIAN ON)
else ()
	message(STATUS "__ST__: Compiler defs: Little-endian")
	add_compile_definitions(LIBSANTAGGER_CFG_WORDS_BIGENDIAN=0)
endif ()

# include the CheckFunctionExists module
include(CheckFunctionExists)

# check if fseeko() exists
check_function_exists(fseeko Z_COMPDEFS_LIBSANTAGGER_HAVE_FSEEKO)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_FSEEKO)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_FSEEKO=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_FSEEKO=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_FSEEKO")
endif ()

# check if readlink() exists
check_function_exists(readlink Z_COMPDEFS_LIBSANTAGGER_HAVE_READLINK)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_READLINK)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_READLINK=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_READLINK=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_READLINK")
endif ()

# check if gettimeofday() exists
check_function_exists(gettimeofday Z_COMPDEFS_LIBSANTAGGER_HAVE_GETTIMEOFDAY)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_GETTIMEOFDAY)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_GETTIMEOFDAY=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_GETTIMEOFDAY=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_GETTIMEOFDAY")
endif ()

# check if nanosleep() exists
check_function_exists(nanosleep Z_COMPDEFS_LIBSANTAGGER_HAVE_NANOSLEEP)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_NANOSLEEP)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_NANOSLEEP=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_NANOSLEEP=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_NANOSLEEP")
endif ()

# include the CheckStructHasMember module
include(CheckStructHasMember)

# check if 'struct timeval' has the member 'tv_sec'
check_struct_has_member("struct timeval" tv_sec sys/select.h Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC LANGUAGE C)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC")
endif ()
# check if 'struct timeval' has the member 'tv_usec'
check_struct_has_member("struct timeval" tv_usec sys/select.h Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC LANGUAGE C)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC")
endif ()

# check if 'struct timespec' has the member 'tv_sec'
check_struct_has_member("struct timespec" tv_sec sys/time.h Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC")
endif ()
# check if 'struct timespec' has the member 'tv_nsec'
check_struct_has_member("struct timespec" tv_nsec sys/time.h Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC)
if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC)
	message(STATUS "__ST__: Compiler defs: LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC=1")
	add_compile_definitions(LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC=1)
else ()
	message(STATUS "__ST__: Compiler defs: no LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC")
endif ()

# ----------------------------------------------------------------------------
# Git Semantic Version
# ----------------------------------------------------------------------------

ts_git_read_version_and_write_to_file(
		"__ST__"
		"${CMAKE_SOURCE_DIR}/.git"
		"${CMAKE_SOURCE_DIR}/src/includes/lib_version.h.in"
		"${CMAKE_SOURCE_DIR}/src/includes/lib_version.h"
		"ST_LIBSANTAG"
)
target_sources(santagger_includes
	INTERFACE FILE_SET HEADERS
	BASE_DIRS ${PROJECT_SOURCE_DIR}
	FILES
		src/includes/lib_version.h
)

ts_git_read_version_and_write_to_file(
		"__ST__"
		"${CMAKE_SOURCE_DIR}/.git"
		"${CMAKE_SOURCE_DIR}/src/app/app_version.h.in"
		"${CMAKE_SOURCE_DIR}/src/app/app_version.h"
		"APP_ST"
)
target_sources(santagger_app
	PRIVATE
		src/app/app_version.h
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
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMPG123 OFF)
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMAD OFF)
if (LIBMPG123 AND LIBMPG123_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libmpg123: ${LIBMPG123}")
	add_compile_definitions(LIBSANTAGGER_HAVE_LIBMPG123=1)
	set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMPG123 ON)

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
		add_compile_definitions(LIBSANTAGGER_HAVE_LIBMAD=1)
		set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMAD ON)

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
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBVORBIS OFF)
if (LIBVORBIS AND LIBVORBIS_INCLUDE_DIR AND LIBOGG AND LIBOGG_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libvorbis: ${LIBVORBIS}")
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libogg: ${LIBOGG}")
	add_compile_definitions(LIBSANTAGGER_HAVE_LIBVORBIS=1)
	set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBVORBIS ON)

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
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBZ OFF)
if (LIBZ AND LIBZ_INCLUDE_DIR)
	message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libz: ${LIBZ}")
	add_compile_definitions(LIBSANTAGGER_HAVE_LIBZ=1)
	set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBZ ON)

	target_link_libraries(santagger_app PRIVATE ${LIBZ})
	include_directories(${LIBZ_INCLUDE_DIR})

	set(Z_VAR_PC_REQU_LIBZ ON)
else ()
	message(STATUS "__ST__: ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libz not found, proceeding without it")
endif ()

# libbcrypt
set(Z_VAR_PC_REQU_BCRYPT OFF)
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBBCRYPT OFF)
if (WIN32)
	# link to shared library 'libbcrypt' - required only for MS Windows
	find_library(LIBBCRYPT NAMES bcrypt)
	if (LIBBCRYPT)
		message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libbcrypt: ${LIBBCRYPT}")
	else ()
		message(FATAL_ERROR "__ST__: libbcrypt not found")
	endif ()
	target_link_libraries(santagger_lib PRIVATE ${LIBBCRYPT})
	set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBBCRYPT ON)

	set(Z_VAR_PC_REQU_BCRYPT ON)
endif ()

# libm
set(Z_VAR_PC_REQU_LIBM OFF)
set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBM OFF)
if (Z_VAR_NEED_LIBM)
	# link to shared library 'libm' - required only for Debian when using libvorbis
	find_library(LIBM NAMES m)
	if (LIBM)
		message(STATUS "__ST__: Found ${Z_VAR_EXT_LIBRARY_LINK_TYPE} libm: ${LIBM}")
		target_link_libraries(santagger_app PRIVATE ${LIBM})
		set(Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBM ON)

		set(Z_VAR_PC_REQU_LIBM ON)
	else ()
		message(STATUS "__ST__: libm not found")
	endif ()
endif ()

# ----------------------------------------------------------------------------
# Create file 'lib_compdefs.h'
# ----------------------------------------------------------------------------

set(X_FN_LIB_COMPDEFS_H "src/includes/lib_compdefs.h")
function (replaceCompDefs)
	set(Z_TEMP_OUTP_FN "${santagger_app_and_lib_project_BINARY_DIR}/${X_FN_LIB_COMPDEFS_H}")
	message(STATUS "__ST__: Writing file '${Z_TEMP_OUTP_FN}'")

	file(READ ${X_FN_LIB_COMPDEFS_H}.in FILE_CONTENTS)
	if (Z_COMPDEFS_LIBSANTAGGER_PPM_FILE_OFFSET_BITS)
		string(REPLACE "//#define LIBSANTAGGER_PPM_FILE_OFFSET_BITS " "#define LIBSANTAGGER_PPM_FILE_OFFSET_BITS " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_PPM_LARGEFILE_SOURCE)
		string(REPLACE "//#define LIBSANTAGGER_PPM_LARGEFILE_SOURCE " "#define LIBSANTAGGER_PPM_LARGEFILE_SOURCE " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (NOT Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_H MATCHES "^$")
		string(REPLACE "//#define LIBSANTAGGER_KWFNC_INLINE_H  some_keyword" "#define LIBSANTAGGER_KWFNC_INLINE_H  ${Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_H}" FILE_CONTENTS ${FILE_CONTENTS})
	else ()
		string(REPLACE "//#define LIBSANTAGGER_KWFNC_INLINE_H  some_keyword" "#define LIBSANTAGGER_KWFNC_INLINE_H  /* empty */" FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (NOT Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S MATCHES "^$")
		string(REPLACE "//#define LIBSANTAGGER_KWFNC_INLINE_S  some_keyword" "#define LIBSANTAGGER_KWFNC_INLINE_S  ${Z_COMPDEFS_LIBSANTAGGER_KWFNC_INLINE_S}" FILE_CONTENTS ${FILE_CONTENTS})
	else ()
		string(REPLACE "//#define LIBSANTAGGER_KWFNC_INLINE_S  some_keyword" "#define LIBSANTAGGER_KWFNC_INLINE_S  /* empty */" FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_CFG_DEBUG)
		string(REPLACE "//#define LIBSANTAGGER_CFG_DEBUG " "#define LIBSANTAGGER_CFG_DEBUG " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_CFG_EXTRA_DEBUG)
		string(REPLACE "//#define LIBSANTAGGER_CFG_EXTRA_DEBUG " "#define LIBSANTAGGER_CFG_EXTRA_DEBUG " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_CFG_HAVE64BIT)
		string(REPLACE "//#define LIBSANTAGGER_CFG_HAVE64BIT " "#define LIBSANTAGGER_CFG_HAVE64BIT " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_CFG_NATIVE64BIT)
		string(REPLACE "//#define LIBSANTAGGER_CFG_NATIVE64BIT " "#define LIBSANTAGGER_CFG_NATIVE64BIT " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_CFG_WORDS_BIGENDIAN)
		string(REPLACE "//#define LIBSANTAGGER_CFG_WORDS_BIGENDIAN " "#define LIBSANTAGGER_CFG_WORDS_BIGENDIAN " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_FSEEKO)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_FSEEKO " "#define LIBSANTAGGER_HAVE_FSEEKO " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_READLINK)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_READLINK " "#define LIBSANTAGGER_HAVE_READLINK " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_GETTIMEOFDAY)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_GETTIMEOFDAY " "#define LIBSANTAGGER_HAVE_GETTIMEOFDAY " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_NANOSLEEP)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_NANOSLEEP " "#define LIBSANTAGGER_HAVE_NANOSLEEP " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC " "#define LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_SEC " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC " "#define LIBSANTAGGER_HAVE_STRUCT_TIMEVAL_TV_USEC " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC " "#define LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_SEC " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC " "#define LIBSANTAGGER_HAVE_STRUCT_TIMESPEC_TV_NSEC " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMPG123)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBMPG123 " "#define LIBSANTAGGER_HAVE_LIBMPG123 " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBMAD)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBMAD " "#define LIBSANTAGGER_HAVE_LIBMAD " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBVORBIS)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBVORBIS " "#define LIBSANTAGGER_HAVE_LIBVORBIS " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBZ)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBZ " "#define LIBSANTAGGER_HAVE_LIBZ " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBBCRYPT)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBBCRYPT " "#define LIBSANTAGGER_HAVE_LIBBCRYPT " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	if (Z_COMPDEFS_LIBSANTAGGER_HAVE_LIBM)
		string(REPLACE "//#define LIBSANTAGGER_HAVE_LIBM " "#define LIBSANTAGGER_HAVE_LIBM " FILE_CONTENTS ${FILE_CONTENTS})
	endif ()
	string(REPLACE "these will be replaced by CMake" "these have been replaced by CMake" FILE_CONTENTS ${FILE_CONTENTS})
	file(WRITE ${Z_TEMP_OUTP_FN} ${FILE_CONTENTS})
endfunction ()
replaceCompDefs()
add_compile_definitions("LIBSANTAGGER_DONT_INCL_LIB_COMPDEFS=1")

# ----------------------------------------------------------------------------
# For Pkg-config and Installation
# ----------------------------------------------------------------------------

include(GNUInstallDirs)

if (CMAKE_INSTALL_PREFIX MATCHES "^$")
	message(FATAL_ERROR "CMAKE_INSTALL_PREFIX is empty")
endif ()
if (CMAKE_INSTALL_FULL_BINDIR MATCHES "^$")
	message(FATAL_ERROR "CMAKE_INSTALL_FULL_BINDIR is empty")
endif ()
if (CMAKE_INSTALL_FULL_LIBDIR MATCHES "^$")
	message(FATAL_ERROR "CMAKE_INSTALL_FULL_LIBDIR is empty")
endif ()
if (CMAKE_INSTALL_FULL_INCLUDEDIR MATCHES "^$")
	message(FATAL_ERROR "CMAKE_INSTALL_FULL_INCLUDEDIR is empty")
endif ()

# ----------------------------------------------------------------------------
# Pkg-config
# ----------------------------------------------------------------------------

if (INSTALL_PKG_CONFIG_MODULE)
	ts_read_git_version_from_c_header_file("__ST__" "${CMAKE_SOURCE_DIR}/src/includes/lib_version.h" "ST_LIBSANTAG_VERS_STRING" TMP_GIT_VERS_SEM TRUE)

	set(TMP_PC_REQU_PRIV "")
	if (BUILD_SHARED_LIBS)
		if (Z_VAR_PC_REQU_MPG123)
			set(TMP_PC_REQU_PRIV "${TMP_PC_REQU_PRIV} libmpg123")
		endif ()
		if (Z_VAR_PC_REQU_MAD)
			set(TMP_PC_REQU_PRIV "${TMP_PC_REQU_PRIV} mad")
		endif ()
		if (Z_VAR_PC_REQU_VORBIS)
			set(TMP_PC_REQU_PRIV "${TMP_PC_REQU_PRIV} vorbis")
		endif ()
		if (Z_VAR_PC_REQU_OGG)
			set(TMP_PC_REQU_PRIV "${TMP_PC_REQU_PRIV} ogg")
		endif ()
		if (Z_VAR_PC_REQU_LIBZ)
			set(TMP_PC_REQU_PRIV "${TMP_PC_REQU_PRIV} zlib")
		endif ()
	endif ()

	set(TMP_PC_LIBS_PRIV "")
	if (BUILD_SHARED_LIBS)
		if (Z_VAR_PC_REQU_BCRYPT)
			set(TMP_PC_LIBS_PRIV "${TMP_PC_LIBS_PRIV} -lbcrypt")
		endif ()
		if (Z_VAR_PC_REQU_LIBM)
			set(TMP_PC_LIBS_PRIV "${TMP_PC_LIBS_PRIV} -lm")
		endif ()
	endif ()

	ts_pkgconfig__configure_pkg_config_file("src" "libsantagger.pc.in" "${OUTPUT_NAME_SUFFIX}" "${TMP_GIT_VERS_SEM}" "${TMP_PC_REQU_PRIV}" "${TMP_PC_LIBS_PRIV}")

	install(
			FILES ${CMAKE_CURRENT_BINARY_DIR}/libsantagger${OUTPUT_NAME_SUFFIX}.pc
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	)
endif ()

# ----------------------------------------------------------------------------
# Installation
# ----------------------------------------------------------------------------

message(STATUS "__ST__: Install directories: PREFIX='${CMAKE_INSTALL_PREFIX}', BIN='${CMAKE_INSTALL_BINDIR}', LIB='${CMAKE_INSTALL_LIBDIR}', INC='${CMAKE_INSTALL_INCLUDEDIR}'")
install(
	TARGETS santagger_app santagger_lib
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
install(
	DIRECTORY src/includes/
	COMPONENT Development
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_ST_PUBLIC_NAME}
	FILES_MATCHING PATTERN "*.h"
)
install(
	FILES
		${santagger_app_and_lib_project_BINARY_DIR}/${X_FN_LIB_COMPDEFS_H}
	COMPONENT Development
	DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${LIBRARY_ST_PUBLIC_NAME}"
)
set_target_properties(santagger_app PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib64")
