#!/usr/bin/env bash
set -e -x
SSCONV_TEST_USE="${SSCONV_TEST_USE:-$1}"
base="Freecell-Solver--benchmarks-vs-time"
tsv="$base.tsv"
perl fc-solve/scripts/plot-benchmarks-against-time.pl | tee "$tsv"
xlsx="$base.xlsx"
cat "$tsv" | (perl -nalE 'say join",",@F[0,2]') | csv2chart xlsx -o "$xlsx" --title "fc-solve Benchmark" --width 1200 --height 600
ods="$base.ods"
ssconvert "$xlsx" "$ods"
gnum="$base.gnumeric"
ssconvert "$xlsx" "$gnum"
svgtemp="i.svg"
svgtemp0="$svgtemp.0"
if test -z "$SSCONV_TEST_USE"
then
    ssconvert --export-graphs "$gnum" "$svgtemp"
elif test "$SSCONV_TEST_USE" = xlsx
then
    ssconvert --export-graphs "$xlsx" "$svgtemp"
elif test "$SSCONV_TEST_USE" = ods
then
    ssconvert --export-graphs "$ods" "$svgtemp"
else
    echo "Unknown format <<$SSCONV_TEST_USE>!"
    exit 1
fi

svg="$base.svg"
mv -f "$svgtemp0" "$svg"
# eog "$svg"
inkscape "$svg"
