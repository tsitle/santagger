#!/usr/bin/env bash

#
# Install application, library and headers
#
# by TS, Sep 2024
#

source zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	{
		echo "Usage: $(basename "${0}") [BUILD_DIR_SUFFIX] [all|<bin>|inc]"
		echo
		echo "If the BUILD_DIR_SUFFIX is empty the default value 'release' will be used."
	} >&2
	exit ${1}
}

LOPT_BUILDDIRSUFFIX="release"
TMP_HAVE_ARG_BUILDDIRSUFFIX=false
LOPT_COMPONENTS="bin"
TMP_HAVE_ARG_COMPONENTS=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "all" ] || [ "${1}" = "bin" ] || [ "${1}" = "inc" ]; then
		if [ "${TMP_HAVE_ARG_COMPONENTS}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'all|bin|inc'\n" >&2
			printUsage 1
		fi
		LOPT_COMPONENTS="${1}"
		TMP_HAVE_ARG_COMPONENTS=true
	elif [ "${1}" = "--help" ]; then
		printUsage 0
	else
		if [ "${TMP_HAVE_ARG_COMPONENTS}" = "false" ]; then
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

TMP_ARG_BUILD_TYPE="$(getCmakeBuildTypeFromSuffix "${LOPT_BUILDDIRSUFFIX}")"
if [ -z "${TMP_ARG_BUILD_TYPE}" ]; then
	echo "$(basename "${0}"): Could not determine BUILD TYPE" >&2
	exit 1
fi

TMP_PC_FN="${TMP_ARG_BUILD_DIR}/lib${GCFG_PROJECT_NAME}.pc"
TMP_ARG_INSTALL_PREFIX="$(grep -e '^prefix=' "${TMP_PC_FN}" | cut -f2 -d=)"

if [ -z "${TMP_ARG_INSTALL_PREFIX}" ]; then
	echo "$(basename "${0}"): Could not read installation prefix from '${TMP_PC_FN}'" >&2
	exit 1
fi

if [ ! -f "${TMP_ARG_BUILD_DIR}/${GCFG_EXECUTABLE_ST_FN}" ]; then
	echo "$(basename "${0}"): No compiled executable found" >&2
	exit 1
fi

command -v sudo >/dev/null 2>&1 && test "$(id -u)" != "0" && TMP_SUDO_EXE="sudo" || TMP_SUDO_EXE=""

if [ -d "${TMP_ARG_INSTALL_PREFIX}/include/${GCFG_PROJECT_NAME}" ]; then
	if [ "${LOPT_COMPONENTS}" = "bin" ]; then
		echo "- Note: '${TMP_ARG_INSTALL_PREFIX}/include/${GCFG_PROJECT_NAME}/' exists - but leaving it alone"
	else
		echo "- Deleting '${TMP_ARG_INSTALL_PREFIX}/include/${GCFG_PROJECT_NAME}/'"
		${TMP_SUDO_EXE} rm -r "${TMP_ARG_INSTALL_PREFIX}/include/${GCFG_PROJECT_NAME}"
	fi
fi

echo "- Installing in '${TMP_ARG_INSTALL_PREFIX}/'"

TMP_ARG_COMP=""
if [ "${LOPT_COMPONENTS}" = "bin" ]; then
	TMP_ARG_COMP="--component Runtime"
elif [ "${LOPT_COMPONENTS}" = "inc" ]; then
	TMP_ARG_COMP="--component Development"
fi

${TMP_SUDO_EXE} "${GCFG_BIN_CMAKE}" \
	--install "${TMP_ARG_BUILD_DIR}" \
	--prefix="${TMP_ARG_INSTALL_PREFIX}" \
	${TMP_ARG_COMP}
