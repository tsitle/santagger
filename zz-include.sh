#
# Common functions and constants
#
# by TS, Sep 2024
#

GCFG_PROJECT_NAME="santagger"

GCFG_OUTPUT_BIN_ARCH_DN="distPre"

GCFG_CMAKE_INSTALL_PREFIX="/opt/${GCFG_PROJECT_NAME}"

GCFG_CMAKE_TARGET_LIB="${GCFG_PROJECT_NAME}_lib"
GCFG_CMAKE_TARGET_APP="${GCFG_PROJECT_NAME}_app"

GCFG_BIN_LIB_STATIC_BASE_FN="lib${GCFG_PROJECT_NAME}-static"
GCFG_BIN_LIB_DYN_BASE_FN="lib${GCFG_PROJECT_NAME}"

GCFG_EXECUTABLE_ST_FN="${GCFG_PROJECT_NAME}"

# ----------------------------------------------------------

# Outputs Operating System name
#
# @return int EXITCODE
function getOsName() {
	case "${OSTYPE}" in
		linux*)
			# check if we are in a 'dockcross' environment
			if [ -n "${CROSS_TRIPLE}" ]; then
				# CROSS_TRIPLE: 'aarch64-w64-mingw32', 'aarch64-unknown-linux-gnu', 'x86_64-w64-mingw32.static'
				if echo -n "${CROSS_TRIPLE}" | grep -q "\-w64\-"; then
					echo -n "win"
				else
					echo -n "linux"
				fi
			else
				echo -n "linux"
			fi
			;;
		darwin*)
			echo -n "macos"
			;;
		msys*)
			echo -n "win"
			;;
		*)
			echo "Error: getOsName(): Unknown OSTYPE '${OSTYPE}'" >&2
			return 1
			;;
	esac
	return 0
}

getOsName >/dev/null || exit 1

GVAR_OS="$(getOsName)"

# ----------------------------------------------------------

# Outputs CPU Architecture
#
# @return int EXITCODE
function _getCpuArch() {
	local TMP_CPUARCH_INP
	# check if we are in a 'dockcross' environment
	if [ -n "${CROSS_TRIPLE}" ]; then
		# CROSS_TRIPLE: 'aarch64-w64-mingw32', 'aarch64-unknown-linux-gnu', 'x86_64-w64-mingw32.static'
		TMP_CPUARCH_INP="$(echo -n "${CROSS_TRIPLE}" | cut -f1 -d-)"
	else
		TMP_CPUARCH_INP="$(uname -m)"
	fi
	#
	case "${TMP_CPUARCH_INP}" in
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
				echo "Error: Unknown CPU architecture '$(uname -m)'" >&2
				return 1
				;;
	esac
	return 0
}

_getCpuArch >/dev/null || exit 1

GVAR_ARCH="$(_getCpuArch)"

# ----------------------------------------------------------

GCFG_BIN_CMAKE=""
if command -v cmake >/dev/null; then
	GCFG_BIN_CMAKE="cmake"
else
	if [ "${GVAR_OS}" = "macos" ] && [ -x "${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/${GVAR_ARCH}/bin/cmake" ]; then
		GCFG_BIN_CMAKE="${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/${GVAR_ARCH}/bin/cmake"
	elif [ "${GVAR_OS}" = "linux" ]; then
		TMP_FIND_FN="tmp.find-$$.tmp"
		test -f "${TMP_FIND_FN}" && rm "${TMP_FIND_FN}"
		find /opt -maxdepth 1 -type d -name "clion-*" | sort -r > "${TMP_FIND_FN}"
		while IFS= read -r TMP_DN; do
			GCFG_BIN_CMAKE="$(find "${TMP_DN}" -type f -name "cmake" -perm -u=x 2>/dev/null)"
			break
		done < "${TMP_FIND_FN}"
		rm "${TMP_FIND_FN}"
	fi
fi

if [ -z "${GCFG_BIN_CMAKE}" ]; then
	echo "Couldn't find cmake. Aborting." >&2
	exit 1
fi

if [ "${GVAR_OS}" = "macos" ]; then
	GCFG_BUILD_DIR_PREFIX="cmake-build-manual-macos"
elif [ "${GVAR_OS}" = "win" ]; then
	GCFG_BUILD_DIR_PREFIX="cmake-build-manual-win"
else
	GCFG_BUILD_DIR_PREFIX="cmake-build-manual-lx"
fi
GCFG_BUILD_DIR_PREFIX+="-${GVAR_ARCH}"

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

	echo -n "${GCFG_BUILD_DIR_PREFIX}-${TMP_C_I_P}"
}

# param $1: Build directory suffix (e.g. "release" or "debug" or "vg_debug" or "vg_debug_stat" or "vg_debug_strip" or "vg_debug_stat_strip")
function getCmakeBuildDirFromSuffix() {
	local TMP_BUILD_D_S_LC
	TMP_BUILD_D_S_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"
	echo -n "${GCFG_BUILD_DIR_PREFIX}-${TMP_BUILD_D_S_LC}"
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

	echo -n "${GCFG_CMAKE_INSTALL_PREFIX}"
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
# param $2: Is for Valgrind? ("true" or "false")
# param $3: File ID (e.g. 'BIN_LIB_STATIC' or 'BIN_LIB_DYN')
function getBinaryFn() {
	local TMP_BASE
	local TMP_BT
	local TMP_VG
	local TMP_FILEID
	TMP_BT="${1}"
	TMP_VG="${2}"
	TMP_FILEID="${3}"
	if [ "${TMP_FILEID}" = "BIN_LIB_STATIC" ]; then
		TMP_BASE="${GCFG_BIN_LIB_STATIC_BASE_FN}"
	elif [ "${TMP_FILEID}" = "BIN_LIB_DYN" ]; then
		TMP_BASE="${GCFG_BIN_LIB_DYN_BASE_FN}"
	else
		echo "Error: getBinaryFn(): Unknown FILE_ID '${TMP_FILEID}'" >&2
		exit 1
	fi
	if [ -z "${TMP_BASE}" ]; then
		echo "Error: getBinaryFn(): Empty base fn for FILE_ID '${TMP_FILEID}'" >&2
		exit 1
	fi
	local TMP_SUFF
	TMP_SUFF=""
	test "${TMP_VG}" = "true" && TMP_SUFF+="vg_"
	if [ -n "${TMP_SUFF}" ] || [ "${TMP_BT}" != "release" ]; then
		TMP_SUFF+="${TMP_BT}"
	fi

	test -n "${TMP_SUFF}" && TMP_BASE+="-${TMP_SUFF}"

	if [ "${TMP_FILEID}" = "BIN_LIB_STATIC" ]; then
		TMP_BASE+=".a"
	else
		if [ "${GVAR_OS}" = "linux" ]; then
			TMP_BASE+=".so"
		elif [ "${GVAR_OS}" = "macos" ]; then
			TMP_BASE+=".dylib"
		else
			echo "Error: getBinaryFn(): Unknown OS '${GVAR_OS}'" >&2
			exit 1
		fi
	fi
	echo -n "${TMP_BASE}"
}
