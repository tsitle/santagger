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
	case "$OSTYPE" in
		linux*)
			echo -n "linux"
			;;
		darwin*)
			echo -n "macos"
			;;
		*)
			echo "${VAR_MYNAME}: Error: getOsName(): Unknown OSTYPE '$OSTYPE'" >>/dev/stderr
			return 1
			;;
	esac
	return 0
}

getOsName >/dev/null || exit 1

# ----------------------------------------------------------

LCFG_BIN_CMAKE=""
if command -v cmake >/dev/null; then
	LCFG_BIN_CMAKE="cmake"
else
	LTMP_OS="$(getOsName)"
	if [ "$LTMP_OS" = "macos" -a -x "${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/x64/bin/cmake" ]; then
		LCFG_BIN_CMAKE="${HOME}/Applications/CLion.app/Contents/bin/cmake/mac/x64/bin/cmake"
	elif [ "$LTMP_OS" = "linux" ]; then
		TMP_FIND_FN="tmp.find-$$.tmp"
		test -f "$TMP_FIND_FN" && rm "$TMP_FIND_FN"
		find /opt -maxdepth 1 -type d -name "clion-*" | sort -r > "$TMP_FIND_FN"
		while IFS= read -r TMP_DN; do
			LCFG_BIN_CMAKE="$(find "$TMP_DN" -type f -name "cmake" -perm -u=x 2>/dev/null)"
			break
		done < "$TMP_FIND_FN"
		rm "$TMP_FIND_FN"
	fi
fi

if [ -z "$LCFG_BIN_CMAKE" ]; then
	echo "Couldn't find cmake. Aborting." >>/dev/stderr
	exit 1
fi

if [ "$LTMP_OS" = "macos" ]; then
	LCFG_BUILD_DIR_PREFIX="cmake-build-macos"
else
	LCFG_BUILD_DIR_PREFIX="cmake-build-lx"
fi

# param $1: Build type (e.g. "Release" or "Debug")
function getCmakeBuildDir() {
	local TMP_BUILD_TYPE_LC
	TMP_BUILD_TYPE_LC="$(echo -n "$1" | tr "[:upper:]" "[:lower:]")"
	echo -n "${LCFG_BUILD_DIR_PREFIX}-${TMP_BUILD_TYPE_LC}"
}
