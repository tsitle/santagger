#!/usr/bin/env bash

#
# Run application in build directory
#
# by TS, Sep 2024
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	{
		echo "Usage   : $(basename "${0}") [BUILD_DIR_SUFFIX] -- [ARGS FOR SANTAGGER]"
		echo "Examples: $(basename "${0}")"
		echo "          $(basename "${0}") debug"
		echo "          $(basename "${0}") vg_debug -- some_audio.mp3"
		echo "          $(basename "${0}") -- -b 'Some Band' some_audio.mp3"
		echo
		echo "If the BUILD_DIR_SUFFIX is empty the default value 'release' will be used."
	} >>/dev/stderr
	exit ${1}
}

LOPT_BUILDDIRSUFFIX="release"
TMP_HAVE_ARG_BUILDDIRSUFFIX=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "--help" ]; then
		printUsage 0
	elif [ "${1}" = "--" ]; then
		shift
		break
	else
		if [ "${TMP_HAVE_ARG_BUILDDIRSUFFIX}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg BUILD_DIR_SUFFIX" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDDIRSUFFIX="${1}"
		TMP_HAVE_ARG_BUILDDIRSUFFIX=true
	fi
	shift
done

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDirFromSuffix "${LOPT_BUILDDIRSUFFIX}")"

LTMP_EXE_FN="${GCFG_EXECUTABLE_ST_FN}"

if [ ! -x "${TMP_ARG_BUILD_DIR}/${LTMP_EXE_FN}" ]; then
	echo -e "$(basename "${0}"): Executable '${TMP_ARG_BUILD_DIR}/${LTMP_EXE_FN}' not found" >>/dev/stderr
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
	TMP_EXE_VG+=" --track-origins=yes"
	#TMP_EXE_VG+=" --undef-value-errors=no"
	#TMP_EXE_VG+=" --verbose"
	TMP_EXE_VG+=" --error-exitcode=1"
fi

# shellcheck disable=SC2048
${TMP_EXE_VG} "${TMP_ARG_BUILD_DIR}/${LTMP_EXE_FN}" "${@}"
