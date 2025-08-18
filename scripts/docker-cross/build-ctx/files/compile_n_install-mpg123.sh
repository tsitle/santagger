#!/usr/bin/env bash

if [ -z "${CROSS_ROOT}" ] || [ -z "${CROSS_TRIPLE}" ]; then
	echo "Env var CROSS_ROOT and/or CROSS_TRIPLE not set" >&2
	exit 1
fi

if [ "${CROSS_TRIPLE}" != "x86_64-linux-gnu" ]; then
	# we are cross-compiling
	LTMP_INST_PREFIX="${CROSS_ROOT}/${CROSS_TRIPLE}"
else
	# we are only pseudo cross-compiling
	LTMP_INST_PREFIX="/usr/local"
fi

if [ ! -d "${LTMP_INST_PREFIX}" ]; then
	echo "Directory '${LTMP_INST_PREFIX}' not found" >&2
	exit 1
fi

# will compile both a shared and a static library

./configure \
	--build=x86_64 \
	--host=${CROSS_TRIPLE} \
	"--prefix=${LTMP_INST_PREFIX}" \
	--enable-static \
	|| exit 1
make -j4 src/libmpg123/libmpg123.la || exit 1
make install
