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
	echo "Usage: $(basename "${0}") [debug|<release>] [all|<bin>|inc]" >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_COMPONENTS="bin"
TMP_HAVE_ARG_COMPONENTS=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|release'" >>/dev/stderr
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

if [ ! -f "${TMP_ARG_BUILD_DIR}/santagger" ]; then
	./zb-build.sh "${LOPT_BUILDTYPE}" || exit 1
fi

if [ -f "${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger" ]; then
	if [ "${LOPT_COMPONENTS}" = "inc" ]; then
		echo "- Note: '${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger' exists - but leaving it alone"
	else
		echo "- Deleting '${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger'"
		rm "${LCFG_CMAKE_INSTALL_PREFIX}/bin/santagger"
	fi
fi
if [ -f "${LCFG_CMAKE_INSTALL_PREFIX}/lib64/libsantagger.so" ]; then
	if [ "${LOPT_COMPONENTS}" = "inc" ]; then
		echo "- Note: '${LCFG_CMAKE_INSTALL_PREFIX}/lib64/libsantagger.*' exists - but leaving it alone"
	else
		echo "- Deleting '${LCFG_CMAKE_INSTALL_PREFIX}/lib64/libsantagger.*'"
		rm "${LCFG_CMAKE_INSTALL_PREFIX}/lib64"/libsantagger.*
	fi
fi
if [ -f "${LCFG_CMAKE_INSTALL_PREFIX}/lib/libsantagger.dylib" ]; then
	if [ "${LOPT_COMPONENTS}" = "inc" ]; then
		echo "- Note: '${LCFG_CMAKE_INSTALL_PREFIX}/lib/libsantagger.*' exists - but leaving it alone"
	else
		echo "- Deleting '${LCFG_CMAKE_INSTALL_PREFIX}/lib/libsantagger.*'"
		rm "${LCFG_CMAKE_INSTALL_PREFIX}/lib"/libsantagger.*
	fi
fi
if [ -d "${LCFG_CMAKE_INSTALL_PREFIX}/include/santagger" ]; then
	if [ "${LOPT_COMPONENTS}" = "bin" ]; then
		echo "- Note: '${LCFG_CMAKE_INSTALL_PREFIX}/include/santagger/' exists - but leaving it alone"
	else
		echo "- Deleting '${LCFG_CMAKE_INSTALL_PREFIX}/include/santagger/'"
		rm -r "${LCFG_CMAKE_INSTALL_PREFIX}/include/santagger"
	fi
fi

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
