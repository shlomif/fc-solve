#!/bin/sh

RSYNC="rsync --progress --verbose --rsh=ssh -r"

make
BERLIOS=BERLIOS make
cd /var/www/html/fc-solve
echo "Uploading to Vipe"
echo
# scp * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
$RSYNC * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
cd /var/www/html/fc-solve-berlios
echo "Uploading to Berlios"
echo
# scp * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/
$RSYNC * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/

