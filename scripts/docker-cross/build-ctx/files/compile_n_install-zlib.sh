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

mkdir cmake-build-docker || exit 1
cd cmake-build-docker || exit 1
cmake \
	-S .. \
	-B . \
	-D "CMAKE_INSTALL_PREFIX=${LTMP_INST_PREFIX}" \
	|| exit 1
make -j4 || exit 1
make install
