#!/usr/bin/env bash

if [ -z "${CROSS_ROOT}" ] || [ -z "${CROSS_TRIPLE}" ]; then
	echo "Env var CROSS_ROOT and/or CROSS_TRIPLE not set" >>/dev/stderr
	exit 1
fi
if [ ! -d "${CROSS_ROOT}/${CROSS_TRIPLE}" ]; then
	echo "Directory '${CROSS_ROOT}/${CROSS_TRIPLE}' not found" >>/dev/stderr
	exit 1
fi

# will compile both a shared and a static library

./configure \
	--build=x86_64 \
	--host=${CROSS_TRIPLE} \
	--prefix=${CROSS_ROOT}/${CROSS_TRIPLE} \
	--enable-static \
	|| exit 1
make -j4 src/libmpg123/libmpg123.la || exit 1
make install
