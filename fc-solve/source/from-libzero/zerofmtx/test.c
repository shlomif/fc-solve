#include <stdio.h>
#include <stdlib.h>
#include <zero/mtx.h>

int
main(int argc, char *argv[])
{
    zerofmtx mtx = FMTXINITVAL;

    if (fmtxtrylk(&mtx)) {
        fprintf(stderr, "fmtxtrylk() succeeded\n");
        fmtxunlk(&mtx);
    }
    fmtxlk(&mtx);
    fprintf(stderr, "fmtxlk()\n");
    fmtxunlk(&mtx);
    fprintf(stderr, "fmtxunlk()\n");

    exit(0);
}

