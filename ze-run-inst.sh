#!/usr/bin/env bash

#
# Run application in installation directory
#
# by TS, Sep 2024
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	echo "Usage: $(basename "${0}") [debug|vg_debug|<release>|vg_release] -- [ARGS FOR SANTAGGER...]" >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
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

# ----------------------------------------------------------

# adjust install prefix
if [ "${LOPT_BUILDTYPE}" != "release" ]; then
	LCFG_CMAKE_INSTALL_PREFIX="${LCFG_CMAKE_INSTALL_PREFIX}-${LOPT_BUILDTYPE}"
fi

#
if [ ! -f "${LCFG_CMAKE_INSTALL_PREFIX}/bin/${LCFG_EXECUTABLE_ST_FN}" ]; then
	echo "$(basename "${0}"): file '${LCFG_CMAKE_INSTALL_PREFIX}/bin/${LCFG_EXECUTABLE_ST_FN}' not found" >>/dev/stderr
	exit 1
fi

export LD_LIBRARY_PATH=${LCFG_CMAKE_INSTALL_PREFIX}/lib64:${LCFG_CMAKE_INSTALL_PREFIX}/lib:${LD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH=${LCFG_CMAKE_INSTALL_PREFIX}/lib:${DYLD_LIBRARY_PATH}

TMP_BIN="${LCFG_CMAKE_INSTALL_PREFIX}/bin/${LCFG_EXECUTABLE_ST_FN}"
echo "$(basename "${0}"): Running from '${TMP_BIN}'"

"${TMP_BIN}" "${@}"
