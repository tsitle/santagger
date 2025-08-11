#!/usr/bin/env bash

# Generate CMakeLists.txt files for Tests
#
# by TS, Aug 2025

LCFG_TARGET_LIB="santagger_lib"
LCFG_TARGET_INCS="santagger_includes"

LCFG_TEST_NAMES=""
LCFG_TEST_NAMES+=" binobj"
LCFG_TEST_NAMES+=" dl"
LCFG_TEST_NAMES+=" m64"
LCFG_TEST_NAMES+=" mtes"
LCFG_TEST_NAMES+=" stream"
LCFG_TEST_NAMES+=" strrd"
LCFG_TEST_NAMES+=" strwr"
LCFG_TEST_NAMES+=" sysfile"
LCFG_TEST_NAMES+=" sysfnc"
LCFG_TEST_NAMES+=" tfldmap"

LCFG_CMAKELISTS_FN="CMakeLists.txt"

appendCmakeListsForTest() {
	{
		echo "target_sources(santagger_test_${1}"
		echo -e "\tPRIVATE"
		echo -e "\t\tfncs_test_common.c"
		echo -e "\t\tfncs_test_common.h"
		echo -e "\t\ttest_${1}.c"
		echo -e ")"
		echo -e "target_link_libraries(santagger_test_${1} PRIVATE ${LCFG_TARGET_LIB} ${LCFG_TARGET_INCS})"
		echo
	} >> "${LCFG_CMAKELISTS_FN}"
}

echo "$(basename "${0}"): create '$(pwd)/${LCFG_CMAKELISTS_FN}'"

test -f "${LCFG_CMAKELISTS_FN}" && rm "${LCFG_CMAKELISTS_FN}"

for TMP_TN in ${LCFG_TEST_NAMES}; do
	[ -z "${TMP_TN}" ] && continue
	#
	appendCmakeListsForTest "${TMP_TN}"
done
