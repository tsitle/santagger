#!/usr/bin/env bash

#
# Create CMake build directory
#
# by TS, Sep 2024
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	echo "Usage: $(basename "${0}") [debug|vg_debug|<release>|vg_release] [strip] [static]" >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_STRIP=""
TMP_HAVE_ARG_STRIP=false
LOPT_STATIC=""
TMP_HAVE_ARG_STATIC=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="$1"
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
		echo -e "$(basename "$0"): Invalid arg '${1}'" >>/dev/stderr
		printUsage 1
	fi
	shift
done

if [ $# -ne 0 ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDir "${LOPT_BUILDTYPE}")"
TMP_ARG_BUILD_TYPE="$(echo -n "${LOPT_BUILDTYPE}" | cut -c1 | tr "[:lower:]" "[:upper:]")$(echo -n "${LOPT_BUILDTYPE}" | cut -c2-)"

TMP_ARG_DO_VALGRIND="-D BUILD_FOR_VALGRIND=OFF"
if [ "${LOPT_BUILDTYPE}" = "vg_debug" ] || [ "${LOPT_BUILDTYPE}" = "vg_release" ]; then
	TMP_ARG_DO_VALGRIND="-D BUILD_FOR_VALGRIND=ON"
fi

if [ "${LOPT_BUILDTYPE}" = "debug" ] || [ "${LOPT_BUILDTYPE}" = "vg_debug" ]; then
	TMP_ARG_BUILD_TYPE="Debug"
elif [ "${LOPT_BUILDTYPE}" = "release" ] || [ "${LOPT_BUILDTYPE}" = "vg_release" ]; then
	TMP_ARG_BUILD_TYPE="Release"
else
	echo -e "$(basename "$0"): Invalid build type '${LOPT_BUILDTYPE}'" >>/dev/stderr
	exit 1
fi

TMP_ARG_DO_STRIP=""
if [ -n "${LOPT_STRIP}" ]; then
	TMP_ARG_DO_STRIP="-D STRIP_AFTER_BUILD=ON"
fi

TMP_ARG_DO_STATIC="-D BUILD_SHARED_LIBS=ON"
if [ -n "${LOPT_STATIC}" ]; then
	TMP_ARG_DO_STATIC="-D BUILD_SHARED_LIBS=OFF"
fi

# adjust install prefix
if [ "${LOPT_BUILDTYPE}" != "release" ]; then
	LCFG_CMAKE_INSTALL_PREFIX="${LCFG_CMAKE_INSTALL_PREFIX}-${LOPT_BUILDTYPE}"
fi

#
if [ -n "${TMP_ARG_BUILD_DIR}" ] && [ -d "${TMP_ARG_BUILD_DIR}" ]; then
	echo "$(basename "$0"): Chown'ing the build directory '${TMP_ARG_BUILD_DIR}'"
	sudo chown -R "$(id -u):$(id -g)" "${TMP_ARG_BUILD_DIR}" || exit 1
fi

"${LCFG_BIN_CMAKE}" \
	--fresh \
	-D CMAKE_BUILD_TYPE="${TMP_ARG_BUILD_TYPE}" \
	${TMP_ARG_DO_STRIP} \
	${TMP_ARG_DO_STATIC} \
	${TMP_ARG_DO_VALGRIND} \
	-D CMAKE_INSTALL_PREFIX="${LCFG_CMAKE_INSTALL_PREFIX}" \
	-S . \
	-B "${TMP_ARG_BUILD_DIR}" \
	|| exit 1
