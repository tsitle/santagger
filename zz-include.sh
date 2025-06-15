#
# Common functions and constants
#
# by TS, Sep 2024
#

LCFG_CMAKE_INSTALL_PREFIX="/opt/santaggg"

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
function getCmakeBuildDir() {
	local TMP_BUILD_TYPE_LC
	TMP_BUILD_TYPE_LC="$(echo -n "${1}" | tr "[:upper:]" "[:lower:]")"
	echo -n "${LCFG_BUILD_DIR_PREFIX}-${TMP_BUILD_TYPE_LC}"
}
