#!/usr/bin/env bash

#
# by TS, 2010, 2025
#

X_OUTFN="README-usermanual-.md"

X_TITLE="Sandy Tagger User Manual"

####

# param $1: Index of help page
# param $2: Arguments for santagger
prOneHelp()
{
	{
		echo "* * *"
		echo
		echo "## <a name=\"hp$1\"></a>\[$2\]"
		echo
		echo "[Back To Top](#top)"
		echo
		echo "\`\`\`"
		santagger $2
		local l_ret=$?
		echo "\`\`\`"
		echo

		[ ${l_ret} -ne 0 ] && {
			echo "invalid ret code" >>/dev/stderr
			exit 1
		}
	} >> "${X_OUTFN}"
}

test ! -d "src" && cd ..
cd src || exit 1
cd .. || exit 1

echo "Writing to '${X_OUTFN}'..."

[ -f "${X_OUTFN}" ] && rm "${X_OUTFN}"

####
declare -a hlppgsA
hlppgsA[${#hlppgsA[@]}]="--help"
hlppgsA[${#hlppgsA[@]}]="--examples"
hlppgsA[${#hlppgsA[@]}]="--track-nr help"
hlppgsA[${#hlppgsA[@]}]="--disc-nr help"
hlppgsA[${#hlppgsA[@]}]="--genre help"
hlppgsA[${#hlppgsA[@]}]="--genre hv"
hlppgsA[${#hlppgsA[@]}]="--rem-pic help"
hlppgsA[${#hlppgsA[@]}]="--rem-geo help"
hlppgsA[${#hlppgsA[@]}]="--fadd help"
hlppgsA[${#hlppgsA[@]}]="--fadd hvIV2"
hlppgsA[${#hlppgsA[@]}]="--fadd hvAV2"
hlppgsA[${#hlppgsA[@]}]="--fadd hvVOR"
hlppgsA[${#hlppgsA[@]}]="--fset help"
hlppgsA[${#hlppgsA[@]}]="--fset hvIV2"
hlppgsA[${#hlppgsA[@]}]="--fset hvAV2"
hlppgsA[${#hlppgsA[@]}]="--fset hvVOR"
hlppgsA[${#hlppgsA[@]}]="--dset help"
hlppgsA[${#hlppgsA[@]}]="--dset hv"
hlppgsA[${#hlppgsA[@]}]="--rem-tag help"
hlppgsA[${#hlppgsA[@]}]="--extr-tag help"
hlppgsA[${#hlppgsA[@]}]="--extr-pic help"
hlppgsA[${#hlppgsA[@]}]="--extr-geo help"
hlppgsA[${#hlppgsA[@]}]="--conv help"
hlppgsA[${#hlppgsA[@]}]="--lev help"
hlppgsA[${#hlppgsA[@]}]="--ebs help"
hlppgsA[${#hlppgsA[@]}]="--tag-sw help"
hlppgsA[${#hlppgsA[@]}]="--tag-ed help"
hlppgsA[${#hlppgsA[@]}]="--tiv2 help"
hlppgsA[${#hlppgsA[@]}]="--pic-tp help"
hlppgsA[${#hlppgsA[@]}]="--pic-fm help"

x_ecnt=${#hlppgsA[@]}

####
{
	echo "# <a name=\"top\"></a>${X_TITLE}"
	echo
	echo "* * *"
	echo
	echo "Index of Help Pages:"
	echo
	for (( i = 0 ; i < x_ecnt ; i++ )) {
		echo "- [${hlppgsA[$i]}](#hp${i})"
	}
	echo
} >> "${X_OUTFN}"
####
for (( i = 0 ; i < x_ecnt ; i++ )) {
	prOneHelp ${i} "${hlppgsA[$i]}"
}
####
X_DATE="$(date "+%F %R")"
{
	echo "* * *"
	echo
	echo "*Generated on ${X_DATE}*"
} >> "${X_OUTFN}"
