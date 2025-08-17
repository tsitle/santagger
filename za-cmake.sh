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
	{
		echo "Usage: $(basename "${0}") [debug|<release>] [vg] [static] [strip] [--prefix=DIRECTORY]"
		echo
		echo "BUILD TYPE:"
		echo "  debug      Build with debug symbols"
		echo "  <release>  Build without debug symbols and with optimization"
		echo
		echo "OPTIONS:"
		echo "  vg         Build for Valgrind (will automatically add 'static' option)"
		echo "  static     Build statically linked lib/executable"
		echo "  strip      Strip lib/executable (only for release and non-Valgrind builds)"
		echo
		echo "INSTALLATION PREFIX:"
		echo "  Default install prefix is '${GCFG_CMAKE_INSTALL_PREFIX}'."
		echo "  Depending on the BUILD TYPE and OPTIONS the prefix will be adjusted."
		echo "  When using a custom prefix like '/usr/local' the prefix will not be adjusted."
	} >>/dev/stderr
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
LOPT_VG=""
TMP_HAVE_ARG_VG=false
LOPT_STRIP=""
TMP_HAVE_ARG_STRIP=false
LOPT_STATIC=""
TMP_HAVE_ARG_STATIC=false
LOPT_INSTALLPREFIX=""
TMP_HAVE_ARG_INSTALLPREFIX=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg BUILDTYPE" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="$1"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "${1}" = "vg" ]; then
		if [ "${TMP_HAVE_ARG_VG}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'vg'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_VG="vg"
		TMP_HAVE_ARG_VG=true
	elif [ "${1}" = "static" ]; then
		if [ "${TMP_HAVE_ARG_STATIC}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'static'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_STATIC="static"
		TMP_HAVE_ARG_STATIC=true
	elif [ "${1}" = "strip" ]; then
		if [ "${TMP_HAVE_ARG_STRIP}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'strip'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_STRIP="strip"
		TMP_HAVE_ARG_STRIP=true
	elif [ "${1}" = "--prefix" ]; then
		if [ "${TMP_HAVE_ARG_INSTALLPREFIX}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg '--prefix'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_INSTALLPREFIX="$(echo -n "${1}" | cut -f2 -d=)"
		TMP_HAVE_ARG_INSTALLPREFIX=true
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

if [ "${TMP_HAVE_ARG_INSTALLPREFIX}" = "true" ]; then
	if [ -z "${LOPT_INSTALLPREFIX}" ] || [ "${LOPT_INSTALLPREFIX}" = "/" ]; then
		LOPT_INSTALLPREFIX="/${GCFG_PROJECT_NAME}"
	fi
	LOPT_INSTALLPREFIX="$(echo -n "${LOPT_INSTALLPREFIX}" | sed -e 's;/$;;')"
	# overwrite global variable
	GCFG_CMAKE_INSTALL_PREFIX="${LOPT_INSTALLPREFIX}"
fi

# ----------------------------------------------------------

# convert first letter of LOPT_BUILDTYPE to uppercase
TMP_ARG_BUILD_TYPE="$(echo -n "${LOPT_BUILDTYPE}" | cut -c1 | tr "[:lower:]" "[:upper:]")$(echo -n "${LOPT_BUILDTYPE}" | cut -c2-)"

# get CMake options
if [ -n "${LOPT_VG}" ] && [ -z "${LOPT_STATIC}" ]; then
	echo "$(basename "$0"): Building for Valgrind requires static build. Adding the 'static' option..."
	LOPT_STATIC="static"
fi
if [ -n "${LOPT_VG}" ] && [ -n "${LOPT_STRIP}" ]; then
	echo "$(basename "$0"): Cannot strip executables/library when building for Valgrind. Disabling the 'strip' option..."
	LOPT_STRIP=""
fi
if [ "${TMP_ARG_BUILD_TYPE}" == "Debug" ] && [ -n "${LOPT_STRIP}" ]; then
	echo "$(basename "$0"): Stripping executables/library with Build Type 'Debug' is nonsensical. Disabling the 'strip' option..."
	LOPT_STRIP=""
fi

test -n "${LOPT_VG}" && TMP_ARG_DO_VALGRIND="-D BUILD_FOR_VALGRIND=ON" || TMP_ARG_DO_VALGRIND="-D BUILD_FOR_VALGRIND=OFF"
test -n "${LOPT_STRIP}" && TMP_ARG_DO_STRIP="-D STRIP_AFTER_BUILD=ON" || TMP_ARG_DO_STRIP=""
test -n "${LOPT_STATIC}" && TMP_ARG_DO_STATIC="-D BUILD_SHARED_LIBS=OFF" || TMP_ARG_DO_STATIC="-D BUILD_SHARED_LIBS=ON"

#
TMP_ARG_BUILD_DIR="$(getCmakeBuildDir "${LOPT_BUILDTYPE}" "${LOPT_VG}" "${LOPT_STATIC}" "${LOPT_STRIP}")"
if [ -z "${LOPT_INSTALLPREFIX}" ]; then
	TMP_ARG_INSTALL_PREFIX="$(getCmakeInstallPrefix "${LOPT_BUILDTYPE}" "${LOPT_VG}")"
else
	TMP_ARG_INSTALL_PREFIX="${LOPT_INSTALLPREFIX}"
fi

# chown build directory
if [ -n "${TMP_ARG_BUILD_DIR}" ] && [ -d "${TMP_ARG_BUILD_DIR}" ]; then
	command -v sudo >/dev/null 2>&1 && test "$(id -u)" != "0" && {
		echo "$(basename "$0"): Chown'ing the build directory '${TMP_ARG_BUILD_DIR}'"
		sudo chown -R "$(id -u):$(id -g)" "${TMP_ARG_BUILD_DIR}" || exit 1
	}
fi

#
"${GCFG_BIN_CMAKE}" \
	--fresh \
	-D CMAKE_BUILD_TYPE="${TMP_ARG_BUILD_TYPE}" \
	${TMP_ARG_DO_VALGRIND} \
	${TMP_ARG_DO_STRIP} \
	${TMP_ARG_DO_STATIC} \
	-D CMAKE_INSTALL_PREFIX="${TMP_ARG_INSTALL_PREFIX}" \
	-S . \
	-B "${TMP_ARG_BUILD_DIR}" \
	|| exit 1
