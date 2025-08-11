#!/usr/bin/env bash

# Generate CMakeLists.txt files for Application
#
# by TS, Sep 2024

LCFG_NAME_LIB="santagger_app"

LCFG_LIB_SUBDIRS=" \
		cln \
		mf \
	"

LCFG_CMAKELISTS_FN="CMakeLists.txt"

createCmakeListsForSubdirectoryEnd() {
	test -f "$LCFG_CMAKELISTS_FN" && rm "$LCFG_CMAKELISTS_FN"

	#
	echo "    create '$(pwd)/$LCFG_CMAKELISTS_FN'"
	{
		echo "target_sources(${LCFG_NAME_LIB}"
		echo -e "\t\tPRIVATE"
	} > "$LCFG_CMAKELISTS_FN"

	find . -type f -mindepth 1 -maxdepth 1 \( -name "*.c" -o -name "*.h" -o -name "*.in" \) -print0 |
			while IFS= read -r -d '' TMP_FN; do
				TMP_FN="$(echo -n "$TMP_FN" | sed -e 's;^\./;;')"
				[ "$TMP_FN" = "app_version.h" ] && continue
				[ "$TMP_FN" = "app_version.h.in" ] && continue
				{
					echo -e "\t\t\t${TMP_FN}"
				} >> "$LCFG_CMAKELISTS_FN"
			done

	{
		echo -e "\t)";
	} >> "$LCFG_CMAKELISTS_FN"
}

# param $1: go into subdirectories?
createCmakeListsForSubdirectoryMaster() {
	LARG_GO_INTO_SUBDIRS="$1"

	test -f "$LCFG_CMAKELISTS_FN" && rm "$LCFG_CMAKELISTS_FN"
	#
	if [ "$LARG_GO_INTO_SUBDIRS" = "true" ]; then
		find . -type d -mindepth 1 -maxdepth 1 -print0 |
				while IFS= read -r -d '' TMP_SUBDIR; do
					TMP_SUBDIR="$(echo -n "$TMP_SUBDIR" | sed -e 's;^\./;;')"
					echo "  sub make CMakeLists -- '$TMP_DN/$TMP_SUBDIR'"
					cd "$TMP_SUBDIR" || exit 1
					#
					createCmakeListsForSubdirectoryEnd
					#
					cd ..
				done
	fi
	#
	TMP_FILE_COUNT="$(find . -type f -mindepth 2 -maxdepth 2 -name "$LCFG_CMAKELISTS_FN" | wc -l | tr -d "[:space:]")"
	test "$TMP_FILE_COUNT" = "0" && TMP_HAVE_SUBS=false || TMP_HAVE_SUBS=true
	#echo "    TMP_FILE_COUNT=$TMP_FILE_COUNT, TMP_HAVE_SUBS=$TMP_HAVE_SUBS"
	#
	TMP_FILE_COUNT="$(find . -type f -maxdepth 1 -regex '.*\.[hc]' | wc -l | tr -d "[:space:]")"
	test "$TMP_FILE_COUNT" = "0" && TMP_HAVE_ROOTFILES=false || TMP_HAVE_ROOTFILES=true
	#echo "    TMP_FILE_COUNT=$TMP_FILE_COUNT, TMP_HAVE_ROOTFILES=$TMP_HAVE_ROOTFILES"
	#
	if [ "$TMP_HAVE_ROOTFILES" = "true" ]; then
		createCmakeListsForSubdirectoryEnd
	fi
	#
	if [ "$TMP_HAVE_SUBS" = "true" ]; then
		if [ "$TMP_HAVE_ROOTFILES" = "true" ]; then
			{
				echo
			} >> "$LCFG_CMAKELISTS_FN"
		fi
		#
		find . -type d -mindepth 1 -maxdepth 1 -print0 |
				while IFS= read -r -d '' TMP_SUBDIR; do
					if [ ! -f "$LCFG_CMAKELISTS_FN" ]; then
						echo "    create '$(pwd)/$LCFG_CMAKELISTS_FN'"
					fi
					TMP_SUBDIR="$(echo -n "$TMP_SUBDIR" | sed -e 's;^\./;;')"
					{
						echo "add_subdirectory(\"${TMP_SUBDIR}\")"
					} >> "$LCFG_CMAKELISTS_FN"
				done
	fi
}

TMP_PWD="$(pwd)"

echo "$0:"

for TMP_DN in $LCFG_LIB_SUBDIRS; do
	[ -z "$TMP_DN" ] && continue
	cd "$TMP_DN" || exit 1
	#
	echo "  master make CMakeLists -- '$TMP_DN'"
	createCmakeListsForSubdirectoryMaster true
	#
	cd "$TMP_PWD" || exit 1
done

echo "  master make CMakeLists -- ROOT"
createCmakeListsForSubdirectoryMaster false
