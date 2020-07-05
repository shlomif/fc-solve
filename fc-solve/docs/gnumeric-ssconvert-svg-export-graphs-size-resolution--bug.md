After running:

```
ssconvert --export-graphs Freecell-Solver--benchmarks-vs-time.xlsx i.svg && mv -f i.svg{.0,}
```

on either [this .xlsx file](https://github.com/shlomif/ssconvert-svg-dims-bug/blob/master/Freecell-Solver--benchmarks-vs-time.xlsx)
which I generated from [the input .tsv](https://github.com/shlomif/ssconvert-svg-dims-bug/blob/master/Freecell-Solver--benchmarks-vs-time.tsv)
using my FOSS [csv2chart program](https://github.com/shlomif/CSV2Chart)
and [Excel::Writer::XLSX](https://metacpan.org/pod/Excel::Writer::XLSX), or its `.gnumeric` analogue,
which contains a large scatter X/Y chart, the `.svg` dimensions are small.

The `.svg` dimensions are better after I use ssconvert to generate an `.ods` and `--export-graph` from it.

* [Reproducing shell program git repo](https://github.com/shlomif/ssconvert-svg-dims-bug)
* [Original program](https://github.com/shlomif/fc-solve/blob/e18fdd9d510517adac10b8cd9adc5315ff262b56/fc-solve/scripts/plot-benchmarks-against-time--gen-charts.bash)

I tested the `*.svg` images viewport sizes in all of:

* inkscape
* firefox
* [eog](https://en.wikipedia.org/wiki/Eye_of_GNOME)
* [eom](https://github.com/mate-desktop/eom)

Note that the chart's dimensions in gnumeric and libreoffice are fine: large ones.

Tested with the git master branch of gnumeric / ssconvert / lib-goffice as well.

I am using [Mageia Linux 8 x86-64](https://en.wikipedia.org/wiki/Mageia)
and [Fedora 32 x86-64](https://fedoramagazine.org/announcing-fedora-32/).

A sample invocation of the repo's program is:

```
( PATH=~/apps/gnumeric/bin:"$PATH" ; bash plot-benchmarks-against-time--gen-charts.bash --viewer "firefox" --input-format xlsx --force false )
( PATH=~/apps/gnumeric/bin:"$PATH" ; bash plot-benchmarks-against-time--gen-charts.bash --viewer "firefox" --input-format ods --force false )
```

I can provide some screenshots if necessary.
