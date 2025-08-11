#!/usr/bin/env bash

# Generate CMakeLists.txt files for App/Library/Includes/Tests
#
# by TS, Sep 2024, Aug 2025

LCFG_SUBDIRS=""
LCFG_SUBDIRS+=" app"
LCFG_SUBDIRS+=" includes"
LCFG_SUBDIRS+=" lib"
LCFG_SUBDIRS+=" tests"

test ! -d "src" && cd ..
cd src || exit 1

for TMP_DN in ${LCFG_SUBDIRS}; do
	[ -z "${TMP_DN}" ] && continue
	cd "${TMP_DN}" || exit 1
	echo -e "\n-- ${TMP_DN}"
	"../scripts/generate_CMakeLists-s-${TMP_DN}.sh" || exit 1
	cd ..
done
