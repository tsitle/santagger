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
		echo "Usage: $(basename "${0}") [debug|vg_debug|<release>|vg_release]"
	} >>/dev/stderr;
	exit ${1}
}

LOPT_BUILDTYPE="release"
TMP_HAVE_ARG_BUILDTYPE=false
while [ $# -ne 0 ]; do
	if [ "${1}" = "debug" ] || [ "${1}" = "release" ] || [ "${1}" = "vg_debug" ] || [ "${1}" = "vg_release" ]; then
		if [ "${TMP_HAVE_ARG_BUILDTYPE}" = "true" ]; then
			echo -e "$(basename "${0}"): Duplicate arg 'debug|...'" >>/dev/stderr
			printUsage 1
		fi
		LOPT_BUILDTYPE="${1}"
		TMP_HAVE_ARG_BUILDTYPE=true
	elif [ "${1}" = "--help" ]; then
		printUsage 0
	elif [ "${1}" = "--" ]; then
		shift
		break
	else
		echo -e "$(basename "${0}"): Invalid arg '${1}'" >>/dev/stderr
		printUsage 1
	fi
	shift
done

# ----------------------------------------------------------

LTMP_TESTFILE_TXT="src/tests/data/data.txt"
LTMP_TESTFILE_BIN="src/tests/data/data.rnd"

if [ ! -f "${LTMP_TESTFILE_TXT}" ]; then
	echo -e "$(basename "${0}"): could not find file '${LTMP_TESTFILE_TXT}'" >>/dev/stderr
	exit 1
fi
if [ ! -f "${LTMP_TESTFILE_BIN}" ]; then
	echo -e "$(basename "${0}"): could not find file '${LTMP_TESTFILE_BIN}'" >>/dev/stderr
	exit 1
fi

#
LTMP_TESTNAME="binobj"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="dl"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- -s || exit 1
echo

#
LTMP_TESTNAME="m64"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="mtes"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="rnd"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" || exit 1
echo

#
LTMP_TESTNAME="stream"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- -s || exit 1
echo

#
LTMP_TESTNAME="strrd"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="strwr"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #1 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_TXT}" || exit 1
echo
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' #2 ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" -- "${LTMP_TESTFILE_BIN}" || exit 1
echo

#
LTMP_TESTNAME="tfldmap"
echo "$(basename "${0}"): TEST '${LTMP_TESTNAME}' ------------------------------------------------"
./zc-run-test.sh "${LOPT_BUILDTYPE}" "${LTMP_TESTNAME}" || exit 1
echo

#
echo "$(basename "${0}"): ALL TESTS PASSED"
