#!/usr/bin/env bash

#
# Build application and library
#
# by TS, Sep 2024
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	{
		echo "Usage: $(basename "${0}") [-v|--verbose] [clean] [BUILD_DIR_SUFFIX] [TARGET]"
		echo
		echo "If the BUILD_DIR_SUFFIX is empty the default value 'release' will be used."
		echo
		echo "Targets:"
		echo "  all"
		echo "  lib"
		echo "  <app>"
		echo "  test_binobj"
		echo "  test_dl"
		echo "  test_m64"
		echo "  test_mtes"
		echo "  test_stream"
		echo "  test_strrd"
		echo "  test_strwr"
		echo "  test_sysfile"
		echo "  test_sysfnc"
		echo "  test_tfldmap"
	} >&2;
	exit ${1}
}

LOPT_CLEAN=false
TMP_HAVE_ARG_CLEAN=false
LOPT_VERBOSE=false
TMP_HAVE_ARG_VERBOSE=false
LOPT_BUILDDIRSUFFIX="release"
TMP_HAVE_ARG_BUILDDIRSUFFIX=false
LOPT_BUILDTARGET="app"
TMP_HAVE_ARG_BUILDTARGET=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "clean" ]; then
		if [ "${TMP_HAVE_ARG_CLEAN}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'clean'\n" >&2
			printUsage 1
		fi
		LOPT_CLEAN=true
		TMP_HAVE_ARG_CLEAN=true
	elif [ "${1}" = "--verbose" ] || [ "${1}" = "-v" ]; then
		if [ "${TMP_HAVE_ARG_VERBOSE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg '-v|--verbose'\n" >&2
			printUsage 1
		fi
		LOPT_VERBOSE=true
		TMP_HAVE_ARG_VERBOSE=true
	elif [ "${1}" = "all" ] || [ "${1}" = "lib" ] || [ "${1}" = "app" ] || \
			[ "${1}" = "test_binobj" ] || [ "${1}" = "test_dl" ] || [ "${1}" = "test_m64" ] || [ "${1}" = "test_mtes" ] || \
			[ "${1}" = "test_stream" ] || [ "${1}" = "test_strrd" ] || [ "${1}" = "test_strwr" ] || \
			[ "${1}" = "test_sysfile" ] || [ "${1}" = "test_sysfnc" ] || \
			[ "${1}" = "test_tfldmap" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTARGET}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg TARGET\n" >&2
			printUsage 1
		fi
		LOPT_BUILDTARGET="${1}"
		TMP_HAVE_ARG_BUILDTARGET=true
	elif [ "${1}" = "--help" ]; then
		printUsage 0
	else
		if [ "${TMP_HAVE_ARG_BUILDTARGET}" = "false" ]; then
			if [ "${TMP_HAVE_ARG_BUILDDIRSUFFIX}" = "true" ]; then
				echo -e "$(basename "${0}"): Duplicate arg BUILD_DIR_SUFFIX\n" >&2
				printUsage 1
			fi
			LOPT_BUILDDIRSUFFIX="${1}"
			TMP_HAVE_ARG_BUILDDIRSUFFIX=true
		else
			echo -e "$(basename "${0}"): Invalid arg '${1}'\n" >&2
			printUsage 1
		fi
	fi
	shift
done

if [ $# -ne 0 ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDirFromSuffix "${LOPT_BUILDDIRSUFFIX}")"

#
TMP_TEST_HAVE_BUILD_FILE=false
if [ -f "${TMP_ARG_BUILD_DIR}/build.ninja" ] || [ -f "${TMP_ARG_BUILD_DIR}/Makefile" ]; then
	TMP_TEST_HAVE_BUILD_FILE=true
fi
if [ ! -d "${TMP_ARG_BUILD_DIR}" ] || [ "${TMP_TEST_HAVE_BUILD_FILE}" != "true" ]; then
	echo "$(basename "${0}"): Build dir '${TMP_ARG_BUILD_DIR}' not found or invalid" >&2
	exit 1
fi

# chown build directory
if [ -n "${TMP_ARG_BUILD_DIR}" ] && [ -d "${TMP_ARG_BUILD_DIR}" ]; then
	command -v sudo >/dev/null 2>&1 && test "$(id -u)" != "0" && {
		echo "$(basename "$0"): Chown'ing the build directory '${TMP_ARG_BUILD_DIR}'"
		sudo chown -R "$(id -u):$(id -g)" "${TMP_ARG_BUILD_DIR}" || exit 1
	}
fi

#
TMP_TARGET_ARG=""
if [ "${LOPT_BUILDTARGET}" = "lib" ]; then
	TMP_TARGET_ARG="--target ${GCFG_CMAKE_TARGET_LIB}"
elif [ "${LOPT_BUILDTARGET}" = "app" ]; then
	TMP_TARGET_ARG="--target ${GCFG_CMAKE_TARGET_APP}"
elif [ "${LOPT_BUILDTARGET}" != "all" ]; then
	TMP_TARGET_ARG="--target ${GCFG_PROJECT_NAME}_${LOPT_BUILDTARGET}"
fi

TMP_ARG=""
[ "${LOPT_CLEAN}" = "true" ] && TMP_ARG="--clean-first"
[ "${LOPT_VERBOSE}" = "true" ] && TMP_ARG+=" --verbose"
"${GCFG_BIN_CMAKE}" \
	--build "${TMP_ARG_BUILD_DIR}" \
	--parallel 4 \
	${TMP_TARGET_ARG} \
	${TMP_ARG} \
	|| exit 1

# ----------------------

if ! { [ "${LOPT_BUILDTARGET}" = "all" ] || [ "${LOPT_BUILDTARGET}" = "app" ]; }; then
	exit 0
fi

cd "${TMP_ARG_BUILD_DIR}" || exit 1

LTMP_EXE_NIX_FN="${GCFG_EXECUTABLE_ST_FN}"
LTMP_EXE_WIN_FN="${GCFG_EXECUTABLE_ST_FN}.exe"

if [ ! -f "${LTMP_EXE_NIX_FN}" ] && [ ! -f "${LTMP_EXE_WIN_FN}" ]; then
	echo "$(basename "${0}"): Executable '${LTMP_EXE_NIX_FN}'/'${LTMP_EXE_WIN_FN}' not found - not creating TAR ball"
	exit 0
fi

test -f "${LTMP_EXE_WIN_FN}" && LTMP_EXE_FN="${LTMP_EXE_WIN_FN}" || LTMP_EXE_FN="${LTMP_EXE_NIX_FN}"

TMP_APP_VERSION_H_FN="../src/includes/lib_version.h"

TMP_GIT_VERSION_SEM="$(grep " ST_LIBSANTAG_VERS_STRING " "${TMP_APP_VERSION_H_FN}" | cut -f2 -d\( | cut -f1 -d\) | tr -d \")"
TMP_GIT_VERSION_CID="$(grep " ST_LIBSANTAG_VERS_COMMITID " "${TMP_APP_VERSION_H_FN}" | cut -f2 -d\( | cut -f1 -d\) | tr -d \")"

if [ -z "${TMP_GIT_VERSION_SEM}" ]; then
	echo "$(basename "${0}"): Could not determine santagger version string" >&2
	exit 1
fi
if [ -z "${TMP_GIT_VERSION_CID}" ]; then
	echo "$(basename "${0}"): Could not determine santagger Commit ID" >&2
	exit 1
fi

LTMP_HOSTNAME="$(hostname)"
test -z "${LTMP_HOSTNAME}" && LTMP_HOSTNAME="unknown"
echo -n "${LTMP_HOSTNAME}" | grep -q "\." && LTMP_HOSTNAME="$(echo -n "${LTMP_HOSTNAME}" | cut -f1 -d.)"

LTMP_FN_BASE="${GCFG_EXECUTABLE_ST_FN}-${LTMP_HOSTNAME}-"
LTMP_FN_BASE+="${GVAR_OS}-${GVAR_ARCH}"
LTMP_FN_BASE+="-${LOPT_BUILDDIRSUFFIX}-${TMP_GIT_VERSION_SEM}"
echo -n "${TMP_GIT_VERSION_SEM}" | grep -q -e "-g${TMP_GIT_VERSION_CID}$" || LTMP_FN_BASE+="-g${TMP_GIT_VERSION_CID}"

# ----------------------

test -d "../${GCFG_OUTPUT_BIN_ARCH_DN}" || mkdir "../${GCFG_OUTPUT_BIN_ARCH_DN}"

if [ "${LTMP_EXE_FN}" = "${LTMP_EXE_WIN_FN}" ]; then
	LTMP_ZIP_FN="${LTMP_FN_BASE}.zip"
	echo "$(basename "${0}"): Creating ZIP file '${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_ZIP_FN}'"
	test -f "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_ZIP_FN}" && rm "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_ZIP_FN}"
	command -v zip >/dev/null 2>&1 && TMP_ZIP_EXE="zip" || TMP_ZIP_EXE="tar.exe"
	if [ "${TMP_ZIP_EXE}" = "zip" ]; then
		"${TMP_ZIP_EXE}" "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_ZIP_FN}" "${LTMP_EXE_FN}" || exit 1
	else
		"${TMP_ZIP_EXE}" acvf "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_ZIP_FN}" "${LTMP_EXE_FN}" || exit 1
	fi
else
	LTMP_TAR_FN="${LTMP_FN_BASE}.tgz"
	echo "$(basename "${0}"): Creating TAR ball '${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_TAR_FN}'"
	test -f "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_TAR_FN}" && rm "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_TAR_FN}"
	tar czf "../${GCFG_OUTPUT_BIN_ARCH_DN}/${LTMP_TAR_FN}" "${LTMP_EXE_FN}" || exit 1
fi
