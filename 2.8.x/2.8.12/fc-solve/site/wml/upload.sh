#!/bin/sh

RSYNC="rsync --progress --verbose --rsh=ssh -r"

make
BERLIOS=BERLIOS make
stable_ver="`./get-recent-stable-version.sh`"
pushd .
cd ./dest
ls freecell-solver-*.tar.gz | 
    grep -vF 'freecell-solver-'"${stable_ver}" |
    grep -vP 'freecell-solver-[0-9]+\.[0-9]*[13579]\.' |
    xargs rm -f
echo "Uploading to Vipe"
echo
# scp * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
$RSYNC * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
popd
pushd .
cd ./dest-berlios
echo "Uploading to Berlios"
echo
# scp * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/
$RSYNC * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/
popd
