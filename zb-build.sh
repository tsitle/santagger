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
	echo "Usage: $(basename "${0}") [-v|--verbose] [clean] [debug|vg_debug|<release>|vg_release] [strip] [static]" >>/dev/stderr
	exit ${1}
}

LOPT_CLEAN=false
TMP_HAVE_ARG_CLEAN=false
LOPT_VERBOSE=false
TMP_HAVE_ARG_VERBOSE=false
LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_STRIP=""
TMP_HAVE_ARG_STRIP=false
LOPT_STATIC=""
TMP_HAVE_ARG_STATIC=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "clean" ]; then
		if [ "${TMP_HAVE_ARG_CLEAN}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'clean'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_CLEAN=true
		TMP_HAVE_ARG_CLEAN=true
	elif [ "${1}" = "--verbose" ] || [ "${1}" = "-v" ]; then
		if [ "${TMP_HAVE_ARG_VERBOSE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg '-v|--verbose'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_VERBOSE=true
		TMP_HAVE_ARG_VERBOSE=true
	elif [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "${1}" = "strip" ]; then
		if [ "${TMP_HAVE_ARG_STRIP}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'strip'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_STRIP="strip"
		TMP_HAVE_ARG_STRIP=true
	elif [ "${1}" = "static" ]; then
		if [ "${TMP_HAVE_ARG_STATIC}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'static'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_STATIC="static"
		TMP_HAVE_ARG_STATIC=true
	elif [ "${1}" = "--help" ]; then
		printUsage 0
	else
		echo -e "$(basename "${0}"): Invalid arg '${1}'" >>/dev/stderr
		printUsage 1
	fi
	shift
done

if [ $# -ne 0 ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDir "${LOPT_BUILDTYPE}")"

TMP_TEST_HAVE_BUILD_FILE=false
if [ -f "${TMP_ARG_BUILD_DIR}/build.ninja" ] || [ -f "${TMP_ARG_BUILD_DIR}/Makefile" ]; then
	TMP_TEST_HAVE_BUILD_FILE=true
fi
if [ ! -d "${TMP_ARG_BUILD_DIR}" ] || [ "${TMP_TEST_HAVE_BUILD_FILE}" != "true" ]; then
	./za-cmake.sh "${LOPT_BUILDTYPE}" ${LOPT_STRIP} ${LOPT_STATIC} || exit 1
fi

TMP_ARG=""
[ "${LOPT_CLEAN}" = "true" ] && TMP_ARG="--clean-first"
[ "${LOPT_VERBOSE}" = "true" ] && TMP_ARG+=" --verbose"
"${LCFG_BIN_CMAKE}" \
	--build "${TMP_ARG_BUILD_DIR}" \
	--parallel 4 \
	${TMP_ARG} \
	|| exit 1
