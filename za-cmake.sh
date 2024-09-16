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
	echo "Usage: $(basename "$0") [debug|<release>]" >>/dev/stderr
	exit $1
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
while [ $# -ne 0 ]; do
	if [ "$1" = "debug" ] || [ "$1" = "release" ]; then
		if [ "$TMP_HAVE_ARG_BUILDTYPE" = "true" ]; then
			echo -e "$(basename "$0"): Duplicate arg 'debug|release'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="$1"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "$1" = "--help" ]; then
		printUsage 0
	else
		echo -e "$(basename "$0"): Invalid arg '$1'" >>/dev/stderr
		printUsage 1
	fi
	shift
done

if [ $# -ne 0 ]; then
	printUsage 1
fi

# ----------------------------------------------------------

TMP_ARG_BUILD_DIR="$(getCmakeBuildDir "$LOPT_BUILDTYPE")"
TMP_ARG_BUILD_TYPE="$(echo -n "$LOPT_BUILDTYPE" | cut -c1 | tr "[:lower:]" "[:upper:]")$(echo -n "$LOPT_BUILDTYPE" | cut -c2-)"

$LCFG_BIN_CMAKE --fresh -DCMAKE_BUILD_TYPE="$TMP_ARG_BUILD_TYPE" -S . -B "$TMP_ARG_BUILD_DIR" || exit 1
