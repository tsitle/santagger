#!/usr/bin/env bash

#
# Install application, library and headers
#
# by TS, Sep 2024
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	echo "Usage: $(basename "${0}") [debug|vg_debug|<release>|vg_release] [all|<bin>|inc]" >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_COMPONENTS="bin"
TMP_HAVE_ARG_COMPONENTS=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "${1}" = "all" ] || [ "${1}" = "bin" ] || [ "${1}" = "inc" ]; then
		if [ "${TMP_HAVE_ARG_COMPONENTS}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'all|bin|inc'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_COMPONENTS="${1}"
		TMP_HAVE_ARG_COMPONENTS=true
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

if [ ! -f "${TMP_ARG_BUILD_DIR}/${LCFG_EXECUTABLE_ST_FN}" ]; then
	./zb-build.sh "${LOPT_BUILDTYPE}" || exit 1
fi

# adjust install prefix
if [ "${LOPT_BUILDTYPE}" != "release" ]; then
	LCFG_CMAKE_INSTALL_PREFIX="${LCFG_CMAKE_INSTALL_PREFIX}-${LOPT_BUILDTYPE}"
fi

#
if [ -d "${LCFG_CMAKE_INSTALL_PREFIX}/include/${LCFG_PROJECT_NAME}" ]; then
	if [ "${LOPT_COMPONENTS}" = "bin" ]; then
		echo "- Note: '${LCFG_CMAKE_INSTALL_PREFIX}/include/${LCFG_PROJECT_NAME}/' exists - but leaving it alone"
	else
		echo "- Deleting '${LCFG_CMAKE_INSTALL_PREFIX}/include/${LCFG_PROJECT_NAME}/'"
		rm -r "${LCFG_CMAKE_INSTALL_PREFIX}/include/${LCFG_PROJECT_NAME}"
	fi
fi

#
echo "- Installing in '${LCFG_CMAKE_INSTALL_PREFIX}/'"

TMP_ARG_COMP=""
if [ "${LOPT_COMPONENTS}" = "bin" ]; then
	TMP_ARG_COMP="--component Runtime"
elif [ "${LOPT_COMPONENTS}" = "inc" ]; then
	TMP_ARG_COMP="--component Development"
fi

"${LCFG_BIN_CMAKE}" \
	--install "${TMP_ARG_BUILD_DIR}" \
	--prefix="${LCFG_CMAKE_INSTALL_PREFIX}" \
	${TMP_ARG_COMP}
