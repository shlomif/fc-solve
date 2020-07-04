#!/usr/bin/env bash
set -e -x
force="${SSCONV_TEST_FORCE:-false}"
while test $# -gt 0; do
	flag="$1"
	shift
	arg="$1"
	shift
	case "$flag" in
	--viewer)
		SSCONV_TEST_VIEWER="$arg"
		;;
	--input-format)
		SSCONV_TEST_USE="$arg"
		;;
	--force)
		force="$arg"
		;;
	*)
		echo "Unknown flag <<$flag>>"
		exit 1
		;;
	esac
done
base="Freecell-Solver--benchmarks-vs-time"
tsv="$base.tsv"
if $force || test \! -e "$tsv"; then
	perl fc-solve/scripts/plot-benchmarks-against-time.pl | tee "$tsv"
fi
xlsx="$base.xlsx"
if $force || test \! -e "$xlsx"; then
	cat "$tsv" | (perl -nalE 'say join",",@F[0,2]') | csv2chart xlsx -o "$xlsx" --title "fc-solve Benchmark" --width 1200 --height 600
fi
ods="$base.ods"
if $force || test \! -e "$ods"; then
	ssconvert "$xlsx" "$ods"
fi
gnum="$base.gnumeric"
if $force || test \! -e "$gnum"; then
	ssconvert "$xlsx" "$gnum"
fi
svgtemp="i.svg"
svgtemp0="$svgtemp.0"
if test -z "$SSCONV_TEST_USE"; then
	ssconvert --export-graphs "$gnum" "$svgtemp"
elif test "$SSCONV_TEST_USE" = xlsx; then
	ssconvert --export-graphs "$xlsx" "$svgtemp"
elif test "$SSCONV_TEST_USE" = ods; then
	ssconvert --export-graphs "$ods" "$svgtemp"
else
	echo "Unknown format <<$SSCONV_TEST_USE>!"
	exit 1
fi

svg="$base.svg"
mv -f "$svgtemp0" "$svg"
viewer="${SSCONV_TEST_VIEWER:-inkscape}"
$viewer "$svg"
