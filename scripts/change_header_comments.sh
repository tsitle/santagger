#!/usr/bin/env bash

# Change header comments
#
# by TS, Sep 2024

function replaceHeaderStuff() {
	while IFS= read -r -d '' TMP_FN; do
		echo "-- $TMP_FN"
		sed 's|(C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)|(C) 2005-2025 by Thomas Sandkuehler (technisandk@gmail.com)|' "$TMP_FN" > "${TMP_FN}-$$" || exit 1
		mv "${TMP_FN}-$$" "${TMP_FN}"
	done
}

find . -type f -regex '.*\.[hc]' -print0 | replaceHeaderStuff
find . -type f -regex '.*\.h\.in' -print0 | replaceHeaderStuff
