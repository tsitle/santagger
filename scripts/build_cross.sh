#!/bin/bash

#
# by TS, Aug 2025
#

test ! -d "src" && cd ..
cd src || exit 1
cd .. || exit 1

VAR_MYNAME="$(basename "$0")"

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
		echo "If the TARGET_OS is empty the application will be built for all target OSes."
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
		echo -e "${VAR_MYNAME}: Invalid TARGET_OS\n" >&2
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
	LTMP_ORG_CROSS_TRG="${CROSS_TRG}"
	test "${CROSS_TRG}" = "${LCNST_TOS_WIN_X64}" && CROSS_TRG="${LCNST_TOS_IMG_WIN_X64}"

	LTMP_TRG_SHORT="$(echo -n "${CROSS_TRG}" | sed -e 's/-static//' -e 's/-shared//' -e 's/-/_/g')"

	LTMP_SCR_FN="run-st_dockcross-${LTMP_TRG_SHORT}.sh"

	if [ ! -x "scripts/docker-cross/${LTMP_SCR_FN}" ]; then
		{
			echo "${VAR_MYNAME}: Script 'scripts/docker-cross/${LTMP_SCR_FN}' not found."
			echo "${VAR_MYNAME}: Perhaps you need to run"
			echo "${VAR_MYNAME}:   $ scripts/docker-cross/build_images.sh ${LTMP_ORG_CROSS_TRG}"
			echo "${VAR_MYNAME}: first."
		} >&2
		exit 1
	fi

	"scripts/docker-cross/${LTMP_SCR_FN}" ./za-cmake.sh static strip --prefix=/usr/local || exit 1
	"scripts/docker-cross/${LTMP_SCR_FN}" ./zb-build.sh release_stat_strip || exit 1
done
