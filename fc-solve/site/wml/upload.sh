#!/bin/sh

make
BERLIOS=BERLIOS make
cd /var/www/html/fc-solve
echo "Uploading to Vipe"
echo
# scp * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
rsync --rsh=ssh * shlomif@vipe.technion.ac.il:public_html/freecell-solver/
cd /var/www/html/fc-solve-berlios
echo "Uploading to Berlios"
echo
scp * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/
# rsync --rsh=ssh * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/

