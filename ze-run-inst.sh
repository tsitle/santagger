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
	echo "Usage: $(basename "${0}") [debug|<release>] -- [ARGS FOR SANTAGGER...]" >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|release'" >>/dev/stderr
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

if [ ! -f "${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger" ]; then
	./zd-install.sh "${LOPT_BUILDTYPE}" || exit 1
fi

export LD_LIBRARY_PATH=${LCFG_CMAKE_INSTALL_PREFIX}/lib64:${LD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH=${LCFG_CMAKE_INSTALL_PREFIX}/lib:${DYLD_LIBRARY_PATH}

TMP_BIN="${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger"
echo "- Running from '${TMP_BIN}'"

"$TMP_BIN" "${@}"
