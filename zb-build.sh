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
	echo "Usage: $(basename "${0}") [-v|--verbose] [clean] [debug|<release>]" >>/dev/stderr
	exit ${1}
}

LOPT_CLEAN=false
TMP_HAVE_ARG_CLEAN=false
LOPT_VERBOSE=false
TMP_HAVE_ARG_VERBOSE=false
LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
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
	elif [ "${1}" = "debug" ] || [ "${1}" = "release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|release'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
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

if [ ! -d "${TMP_ARG_BUILD_DIR}" ]; then
	./za-cmake.sh "${LOPT_BUILDTYPE}" || exit 1
fi

TMP_ARG=""
[ "${LOPT_CLEAN}" = "true" ] && TMP_ARG="--clean-first"
[ "${LOPT_VERBOSE}" = "true" ] && TMP_ARG+=" --verbose"
"${LCFG_BIN_CMAKE}" \
	--build "${TMP_ARG_BUILD_DIR}" \
	--parallel 4 \
	${TMP_ARG} \
	|| exit 1
