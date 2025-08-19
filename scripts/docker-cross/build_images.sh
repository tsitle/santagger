#!/bin/bash

#
# by TS, Aug 2025
#

VAR_MYNAME="$(basename "$0")"

# ----------------------------------------------------------

# Outputs CPU architecture string
#
# @param string $1 debian_rootfs|debian_dist
#
# @return int EXITCODE
function _getCpuArch() {
	case "$(uname -m)" in
		x86_64*)
			echo -n "amd64"
			;;
		aarch64*)
			if [ "$1" = "debian_rootfs" ]; then
				echo -n "arm64v8"
			elif [ "$1" = "debian_dist" ]; then
				echo -n "arm64"
			else
				echo "${VAR_MYNAME}: Error: invalid arg '$1'" >&2
				return 1
			fi
			;;
		armv7*)
			if [ "$1" = "debian_rootfs" ]; then
				echo -n "arm32v7"
			elif [ "$1" = "debian_dist" ]; then
				echo -n "armhf"
			else
				echo "${VAR_MYNAME}: Error: invalid arg '$1'" >&2
				return 1
			fi
			;;
		*)
			echo "${VAR_MYNAME}: Error: Unknown CPU architecture '$(uname -m)'" >&2
			return 1
			;;
	esac
	return 0
}

_getCpuArch debian_dist >/dev/null || exit 1

# ----------------------------------------------------------

if [ -d "scripts/docker-cross/build-ctx" ]; then
	cd scripts/docker-cross/build-ctx || exit 1
elif [ -d "docker-cross/build-ctx" ]; then
	cd docker-cross/build-ctx || exit 1
else
	cd build-ctx || exit 1
fi

# ----------------------------------------------------------

LVAR_DEB_DIST="$(_getCpuArch debian_dist)"

if [ "${LVAR_DEB_DIST}" != "amd64" ]; then
	echo "${VAR_MYNAME}: Error: Only amd64/x86_64 hosts supported by dockcross Docker Images" >&2
	exit 1
fi

# ----------------------------------------------------------

LCNST_TOS_LX_X64="linux-x64"
LCNST_TOS_LX_ARM64="linux-arm64"
LCNST_TOS_WIN_X64="windows-x64"
LCNST_TOS_WIN_ARM64="windows-arm64"

LCNST_TOS_IMG_WIN_X64="windows-static-x64"

printUsage() {
	{
		echo "Usage: ${VAR_MYNAME} [TARGET_OS]"
		echo
		echo "If the TARGET_OS is empty all images will be built."
		echo
		echo "TARGET_OS:"
		echo "  ${LCNST_TOS_LX_X64}"
		echo "  ${LCNST_TOS_LX_ARM64}"
		echo "  ${LCNST_TOS_WIN_X64}"
		echo "  ${LCNST_TOS_WIN_ARM64}"
	} >&2
	exit 1
}

if [ $# -gt 1 ] || { [ $# -eq 1 ] && [ "${1}" = "--help" ]; }; then
	printUsage
fi

LVAR_CROSS_TARGETS=""
if [ $# -eq 1 ]; then
	if ! { [ "${1}" = "${LCNST_TOS_LX_X64}" ] || [ "${1}" = "${LCNST_TOS_LX_ARM64}" ] || \
			[ "${1}" = "${LCNST_TOS_WIN_X64}" ] || [ "${1}" = "${LCNST_TOS_WIN_ARM64}" ]; }; then
		echo "${VAR_MYNAME}: Invalid TARGET_OS" >&2
		echo >&2
		printUsage
	fi
	LVAR_CROSS_TARGETS="$1"
else
	LVAR_CROSS_TARGETS+=" ${LCNST_TOS_LX_X64}"
	LVAR_CROSS_TARGETS+=" ${LCNST_TOS_LX_ARM64}"
	LVAR_CROSS_TARGETS+=" ${LCNST_TOS_WIN_X64}"
	LVAR_CROSS_TARGETS+=" ${LCNST_TOS_WIN_ARM64}"
fi

for CROSS_TRG in $LVAR_CROSS_TARGETS; do
	echo -e "\n${VAR_MYNAME}: Build Docker Image for '${CROSS_TRG}'...\n"

	test "${CROSS_TRG}" = "${LCNST_TOS_WIN_X64}" && CROSS_TRG="${LCNST_TOS_IMG_WIN_X64}"

	LTMP_TRG_SHORT="$(echo -n "${CROSS_TRG}" | sed -e 's/-static//' -e 's/-shared//' -e 's/-/_/g')"
	LVAR_IMAGE_NAME="app-st-dockcross-${LTMP_TRG_SHORT}-${LVAR_DEB_DIST}"
	LVAR_IMAGE_VER="latest"

	LVAR_SRC_OS_IMAGE="dockcross/${CROSS_TRG}"

	docker build \
		--build-arg CF_SRC_OS_IMAGE="${LVAR_SRC_OS_IMAGE}" \
		--build-arg CF_OUTP_IMAGE="${LVAR_IMAGE_NAME}" \
		-t "${LVAR_IMAGE_NAME}":"${LVAR_IMAGE_VER}" \
		. \
		|| exit 1

	LTMP_SCR_FN="run-st_dockcross-${LTMP_TRG_SHORT}.sh"

	docker run --rm "${LVAR_IMAGE_NAME}:${LVAR_IMAGE_VER}" > "../${LTMP_SCR_FN}" || exit 1
	chmod +x "../${LTMP_SCR_FN}"
done
