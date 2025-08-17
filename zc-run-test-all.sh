#!/usr/bin/env bash

#
# Run application in build directory
#
# by TS, Aug 2025
#

. zz-include.sh || exit 1

# ----------------------------------------------------------

# param $1: Exit code
function printUsage() {
	{
		echo "Usage: $(basename "${0}") [BUILD_DIR_SUFFIX]"
		echo
		echo "If the BUILD_DIR_SUFFIX is empty the default value 'release' will be used."
	} >&2;
	exit ${1}
}

LOPT_BUILDDIRSUFFIX="release"
TMP_HAVE_ARG_BUILDDIRSUFFIX=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "--help" ]; then
		printUsage 0
	else
		if [ "${TMP_HAVE_ARG_BUILDDIRSUFFIX}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg BUILD_DIR_SUFFIX\n" >&2
			printUsage 1
		fi
		LOPT_BUILDDIRSUFFIX="${1}"
		TMP_HAVE_ARG_BUILDDIRSUFFIX=true
	fi
	shift
done

# ----------------------------------------------------------

LTMP_TESTFILE_TXT="src/tests/data/data.txt"
LTMP_TESTFILE_BIN="src/tests/data/data.rnd"

if [ ! -f "${LTMP_TESTFILE_TXT}" ]; then
	echo "$(basename "${0}"): could not find file '${LTMP_TESTFILE_TXT}'" >&2
	exit 1
fi
if [ ! -f "${LTMP_TESTFILE_BIN}" ]; then
	echo "$(basename "${0}"): could not find file '${LTMP_TESTFILE_BIN}'" >&2
	exit 1
fi

#
LTMP_TESTNAME="binobj"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="dl"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- -s || exit 1
echo

#
LTMP_TESTNAME="m64"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="mtes"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="stream"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- -s || exit 1
echo

#
LTMP_TESTNAME="strrd"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="strwr"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="sysfile"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="sysfnc"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="tfldmap"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDDIRSUFFIX}" "${LTMP_TESTNAME}" || exit 1
echo

#
echo "$(basename "${0}"): ALL TESTS PASSED"
