#!/usr/bin/env bash

#
# Run application in build directory
#
# by TS, Aug 2025
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	{
		echo "Usage: $(basename "${0}") [BUILD_DIR_SUFFIX] TESTNAME -- [ARGS FOR TEST]"
		echo
		echo "If the BUILD_DIR_SUFFIX is empty the default value 'release' will be used."
		echo
		echo "Tests:"
		echo "  binobj"
		echo "  dl"
		echo "  m64"
		echo "  mtes"
		echo "  stream"
		echo "  strrd"
		echo "  strwr"
		echo "  sysfile"
		echo "  sysfnc"
		echo "  tfldmap"
	} >&2;
	exit ${1}
}

LOPT_BUILDDIRSUFFIX="release"
TMP_HAVE_ARG_BUILDDIRSUFFIX=false
LOPT_TESTNAME=""
TMP_HAVE_ARG_TESTNAME=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "binobj" ] || [ "${1}" = "dl" ] || [ "${1}" = "m64" ] || [ "${1}" = "mtes" ] || \
			[ "${1}" = "stream" ] || [ "${1}" = "strrd" ] || [ "${1}" = "strwr" ] || \
			[ "${1}" = "sysfile" ] || [ "${1}" = "sysfnc" ] || \
			[ "${1}" = "tfldmap" ]; then
		if [ "${TMP_HAVE_ARG_TESTNAME}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg TESTNAME\n" >&2
			printUsage 1
		fi
		LOPT_TESTNAME="${1}"
		TMP_HAVE_ARG_TESTNAME=true
	elif [ "${1}" = "--help" ]; then
		printUsage 0
	elif [ "${1}" = "--" ]; then
		shift
		break
	else
		if [ "${TMP_HAVE_ARG_TESTNAME}" = "false" ]; then
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

if [ -z "${LOPT_TESTNAME}" ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDirFromSuffix "${LOPT_BUILDDIRSUFFIX}")"

LTMP_EXE_FN="${GCFG_PROJECT_NAME}_test_${LOPT_TESTNAME}"

if [ ! -x "${TMP_ARG_BUILD_DIR}/${LTMP_EXE_FN}" ]; then
	echo "$(basename "${0}"): Executable '${TMP_ARG_BUILD_DIR}/${LTMP_EXE_FN}' not found" >&2
	exit 1
fi

export LD_LIBRARY_PATH=${TMP_ARG_BUILD_DIR}:${LD_LIBRARY_PATH}

# run with Valgrind?
TMP_IS_FOR_VG="$(getIsForValgrindFromSuffix "${LOPT_BUILDDIRSUFFIX}")"
TMP_EXE_VG=""
if [ "${TMP_IS_FOR_VG}" = "true" ]; then
	TMP_EXE_VG="valgrind"
	TMP_EXE_VG+=" --leak-check=full"
	TMP_EXE_VG+=" --show-leak-kinds=all"
	#TMP_EXE_VG+=" --track-origins=yes"
	TMP_EXE_VG+=" --undef-value-errors=no"
	#TMP_EXE_VG+=" --verbose"
	TMP_EXE_VG+=" --error-exitcode=1"
fi

${TMP_EXE_VG} "${TMP_ARG_BUILD_DIR}/${GCFG_PROJECT_NAME}_test_${LOPT_TESTNAME}" "${@}"
