#!/usr/bin/env bash

# Change header comments
#
# by TS, Sep 2024

find . -type f -regex '.*\.[hc]' -print0 |
		while IFS= read -r -d '' TMP_FN; do
			echo "-- $TMP_FN"
			sed 's|(C) 2005-2011 by Thomas Sandkuehler (santagger@slur97.org) |(C) 2005-2024 by Thomas Sandkuehler (technisandk@gmail.com)|' "$TMP_FN" > "${TMP_FN}-$$" || exit 1
			mv "${TMP_FN}-$$" "${TMP_FN}"
		done
