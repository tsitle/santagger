#!/usr/bin/env bash

if [ -z "${CROSS_ROOT}" ] || [ -z "${CROSS_TRIPLE}" ]; then
	echo "Env var CROSS_ROOT and/or CROSS_TRIPLE not set" >&2
	exit 1
fi
if [ ! -d "${CROSS_ROOT}/${CROSS_TRIPLE}" ]; then
	echo "Directory '${CROSS_ROOT}/${CROSS_TRIPLE}' not found" >&2
	exit 1
fi

# will compile both a shared and a static library

mkdir cmake-build-docker || exit 1
cd cmake-build-docker || exit 1
cmake \
	-S .. \
	-B . \
	-D CMAKE_INSTALL_PREFIX=${CROSS_ROOT}/${CROSS_TRIPLE} \
	|| exit 1
make -j4 || exit 1
make install
