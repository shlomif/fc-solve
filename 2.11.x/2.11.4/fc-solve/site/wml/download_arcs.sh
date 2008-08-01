cd src
rsync -r --rsh=ssh -v --progress \
    shell.berlios.de:public_html/freecell-solver/\*.zip  \
    shell.berlios.de:public_html/freecell-solver/\*.tar.gz  \
    shell.berlios.de:public_html/freecell-solver/\*.patch \
    .

