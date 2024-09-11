#!/usr/bin/env bash

# Change old include path for shared library header files to new style
#
# by TS, Sep 2024

echo "NOT REQUIRED ANYMORE"
exit 1

find . -type f -regex '.*\.[hc]' -print0 |
		while IFS= read -r -d '' TMP_FN; do
			echo "-- $TMP_FN"
			sed 's|#include <santag/\(.*\)\.h>|#include "src/includes/\1.h"|' "$TMP_FN" > "${TMP_FN}-$$" || exit 1
			mv "${TMP_FN}-$$" "${TMP_FN}"
		done
