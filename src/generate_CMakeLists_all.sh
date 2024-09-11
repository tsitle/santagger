#!/usr/bin/env bash

# Generate CMakeLists.txt files for App/Library/Includes
#
# by TS, Sep 2024

LCFG_SUBDIRS=" \
		app \
		includes \
		lib \
	"

for TMP_DN in $LCFG_SUBDIRS; do
	[ -z "$TMP_DN" ] && continue
	cd "$TMP_DN" || exit 1
	echo -e "\n-- $TMP_DN"
	./generate_CMakeLists_${TMP_DN}.sh || exit 1
	cd ..
done
