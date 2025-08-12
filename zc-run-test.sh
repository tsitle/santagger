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
		echo "Usage: $(basename "${0}") [debug|vg_debug|<release>|vg_release] TESTNAME -- [ARGS FOR TEST...]"
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
	} >>/dev/stderr;
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_TESTNAME=""
TMP_HAVE_ARG_TESTNAME=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "${1}" = "binobj" ] || [ "${1}" = "dl" ] || [ "${1}" = "m64" ] || [ "${1}" = "mtes" ] || \
			[ "${1}" = "stream" ] || [ "${1}" = "strrd" ] || [ "${1}" = "strwr" ] || \
			[ "${1}" = "sysfile" ] || [ "${1}" = "sysfnc" ] || \
			[ "${1}" = "tfldmap" ]; then
		if [ "${TMP_HAVE_ARG_TESTNAME}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg TESTNAME" >>/dev/stderr
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
		echo -e "$(basename "${0}"): Invalid arg '${1}'" >>/dev/stderr
		printUsage 1
	fi
	shift
done

if [ -z "${LOPT_TESTNAME}" ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDir "${LOPT_BUILDTYPE}")"

if [ ! -f "${TMP_ARG_BUILD_DIR}/${LCFG_PROJECT_NAME}_test_${LOPT_TESTNAME}" ]; then
	./zb-build.sh "${LOPT_BUILDTYPE}" "test_${LOPT_TESTNAME}" || exit 1
fi

export LD_LIBRARY_PATH=${TMP_ARG_BUILD_DIR}:${LD_LIBRARY_PATH}

TMP_EXE_VG=""
if [ "${LOPT_BUILDTYPE}" = "vg_debug" ] || [ "${LOPT_BUILDTYPE}" = "vg_release" ]; then
	TMP_EXE_VG="valgrind"
	TMP_EXE_VG+=" --leak-check=full"
	TMP_EXE_VG+=" --show-leak-kinds=all"
	#TMP_EXE_VG+=" --track-origins=yes"
	TMP_EXE_VG+=" --undef-value-errors=no"
	#TMP_EXE_VG+=" --verbose"
	TMP_EXE_VG+=" --error-exitcode=1"
fi

${TMP_EXE_VG} "${TMP_ARG_BUILD_DIR}/${LCFG_PROJECT_NAME}_test_${LOPT_TESTNAME}" "${@}"
