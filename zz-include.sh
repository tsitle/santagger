#
# Common functions and constants
#
# by TS, Sep 2024
#

LCFG_PROJECT_NAME="santagger"

LCFG_CMAKE_INSTALL_PREFIX="/opt/${LCFG_PROJECT_NAME}"

LCFG_CMAKE_TARGET_LIB="${LCFG_PROJECT_NAME}_lib"
LCFG_CMAKE_TARGET_APP="${LCFG_PROJECT_NAME}_app"

LCFG_BIN_LIB_STATIC_BASE_FN="lib${LCFG_PROJECT_NAME}-static"
LCFG_BIN_LIB_DYN_BASE_FN="lib${LCFG_PROJECT_NAME}"

LCFG_EXECUTABLE_ST_FN="${LCFG_PROJECT_NAME}"

# ----------------------------------------------------------

# Outputs Operating System name
#
# @return int EXITCODE
function getOsName() {
	case "${OSTYPE}" in
		linux*)
			echo -n "linux"
			;;
		darwin*)
			echo -n "macos"
			;;
		*)
			echo "Error: getOsName(): Unknown OSTYPE '${OSTYPE}'" >>/dev/stderr
			return 1
			;;
	esac
	return 0
}

getOsName >/dev/null || exit 1

LVAR_OS="$(getOsName)"

# ----------------------------------------------------------

# Outputs CPU Architecture
#
# @return int EXITCODE
function _getCpuArch() {
	case "$(uname -m)" in
		x86_64*)
				echo -n "x64"
				;;
		i686*)
				echo -n "x86"
				;;
		arm64*|aarch64*)  # macOS:arm64, linux:aarch64
				echo -n "aarch64"
				;;
		armv7*)
				echo -n "armhf"
				;;
		*)
				echo "Error: Unknown CPU architecture '$(uname -m)'" >>/dev/stderr
				return 1
				;;
	esac
	return 0
}

_getCpuArch >/dev/null || exit 1

LVAR_ARCH="$(_getCpuArch)"

# ----------------------------------------------------------

LCFG_BIN_CMAKE=""
if command -v cmake >/dev/null; then
	LCFG_BIN_CMAKE="cmake"
else
	if [ "${LVAR_OS}" = "macos" ] && [ -x "${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/${LVAR_ARCH}/bin/cmake" ]; then
		LCFG_BIN_CMAKE="${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/${LVAR_ARCH}/bin/cmake"
	elif [ "${LVAR_OS}" = "linux" ]; then
		TMP_FIND_FN="tmp.find-$$.tmp"
		test -f "${TMP_FIND_FN}" && rm "${TMP_FIND_FN}"
		find /opt -maxdepth 1 -type d -name "clion-*" | sort -r > "${TMP_FIND_FN}"
		while IFS= read -r TMP_DN; do
			LCFG_BIN_CMAKE="$(find "${TMP_DN}" -type f -name "cmake" -perm -u=x 2>/dev/null)"
			break
		done < "${TMP_FIND_FN}"
		rm "${TMP_FIND_FN}"
	fi
fi

if [ -z "${LCFG_BIN_CMAKE}" ]; then
	echo "Couldn't find cmake. Aborting." >>/dev/stderr
	exit 1
fi

if [ "${LVAR_OS}" = "macos" ]; then
	LCFG_BUILD_DIR_PREFIX="cmake-build-manual-macos"
else
	LCFG_BUILD_DIR_PREFIX="cmake-build-manual-lx"
fi

# param $1: Build type (e.g. "Release" or "Debug")
# param $2: Build for Valgrind? (true if non-empty)
# param $3: Build statically linked binary/lib? (true if non-empty)
# param $4: Strip binary/lib? (true if non-empty)
function getCmakeBuildDir() {
	local TMP_BUILD_TYPE_LC
	TMP_BUILD_TYPE_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_C_I_P
	TMP_C_I_P=""
	test -n "${2}" && TMP_C_I_P="vg_"
	TMP_C_I_P+="${TMP_BUILD_TYPE_LC}"
	test -n "${3}" && TMP_C_I_P+="_stat"
	test -n "${4}" && TMP_C_I_P+="_strip"

	echo -n "${LCFG_BUILD_DIR_PREFIX}-${TMP_C_I_P}"
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getCmakeBuildDirFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"
	echo -n "${LCFG_BUILD_DIR_PREFIX}-${TMP_BUILD_D_S_LC}"
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getCmakeBuildTypeFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_HAVE_REL
	TMP_HAVE_REL=false
	echo -n "${TMP_BUILD_D_S_LC}" | grep -q -e '^release$' -e '_release$' -e '^release_' -e '_release_' && TMP_HAVE_REL=true
	if [ "${TMP_HAVE_REL}" = "true" ]; then
		echo -n "release"
	else
		local TMP_HAVE_DBG
		TMP_HAVE_DBG=false
		echo -n "${TMP_BUILD_D_S_LC}" | grep -q -e '^debug$' -e '_debug$' -e '^debug_' -e '_debug_' && TMP_HAVE_DBG=true
		if [ "${TMP_HAVE_DBG}" = "true" ]; then
			echo -n "debug"
		else
			echo -n ""
		fi
	fi
}

# param $1: Build type (e.g. "Release" or "Debug")
# param $2: Build for Valgrind? (true if non-empty)
function getCmakeInstallPrefix() {
	local TMP_BUILD_TYPE_LC
	TMP_BUILD_TYPE_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_C_I_P
	TMP_C_I_P=""
	if [ -n "${2}" ] || [ "${TMP_BUILD_TYPE_LC}" != "release" ]; then
		test -n "${2}" && TMP_C_I_P="vg_"
		TMP_C_I_P+="${TMP_BUILD_TYPE_LC}"
	fi

	echo -n "${LCFG_CMAKE_INSTALL_PREFIX}"
	if [ -n "${TMP_C_I_P}" ]; then
		echo -n "-${TMP_C_I_P}"
	fi
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getIsForValgrindFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_HAVE_VG
	TMP_HAVE_VG=false
	echo -n "${TMP_BUILD_D_S_LC}" | grep -q -e '^vg_' && TMP_HAVE_VG=true
	if [ "${TMP_HAVE_VG}" = "true" ]; then
		echo -n "true"
	else
		echo -n "false"
	fi
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getIsStaticFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_HAVE_STAT
	TMP_HAVE_STAT=false
	echo -n "${TMP_BUILD_D_S_LC}" | grep -q -e '_stat' && TMP_HAVE_STAT=true
	if [ "${TMP_HAVE_STAT}" = "true" ]; then
		echo -n "true"
	else
		echo -n "false"
	fi
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getIsStripFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"

	local TMP_HAVE_STRIP
	TMP_HAVE_STRIP=false
	echo -n "${TMP_BUILD_D_S_LC}" | grep -q -e '_strip' && TMP_HAVE_STRIP=true
	if [ "${TMP_HAVE_STRIP}" = "true" ]; then
		echo -n "true"
	else
		echo -n "false"
	fi
}

# param $1: Build type (e.g. "Release" or "Debug")
# param $2: File ID (e.g. 'BIN_LIB_STATIC' or 'BIN_LIB_DYN')
function getBinaryFn() {
	local TMP_BASE
	if [ "${2}" = "BIN_LIB_STATIC" ]; then
		TMP_BASE="${LCFG_BIN_LIB_STATIC_BASE_FN}"
	elif [ "${2}" = "BIN_LIB_DYN" ]; then
		TMP_BASE="${LCFG_BIN_LIB_DYN_BASE_FN}"
	else
		echo "Error: getBinaryFn(): Unknown FILE_ID '${2}'" >>/dev/stderr
		exit 1
	fi
	if [ -z "${TMP_BASE}" ]; then
		echo "Error: getBinaryFn(): Empty base fn for FILE_ID '${2}'" >>/dev/stderr
		exit 1
	fi
	if [ "${1}" != "release" ]; then
		TMP_BASE+="-${1}"
	fi
	if [ "${2}" = "BIN_LIB_STATIC" ]; then
		TMP_BASE+=".a"
	else
		if [ "${LVAR_OS}" = "linux" ]; then
			TMP_BASE+=".so"
		elif [ "${LVAR_OS}" = "macos" ]; then
			TMP_BASE+=".dylib"
		else
			echo "Error: getBinaryFn(): Unknown OS '${LVAR_OS}'" >>/dev/stderr
			exit 1
		fi
	fi
	echo -n "${TMP_BASE}"
}
