# Copyright (c) 2020 Shlomi Fish ( https://www.shlomifish.org/ )
# Author: Shlomi Fish ( https://www.shlomifish.org/ )
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

set -e
set -o pipefail

ff()
{
	local c="$1"
	shift
	perl ../scripts/time-fcs.pl FCS_SWAP_COLOR=*--count_deals="$c"*.report.txt | perl -lnE 'say "$2\t$1" if /([0-9\.]+)::([0-9]+)$/'
}

fff()
{
	echo "$1"
	ff "$1" | cut -f 1
}

main()
{
    paste <(fff 32000) <(fff 200000) | tsv2csv | csv2chart xlsx -o fff.xlsx --exec gnumeric
}

main
