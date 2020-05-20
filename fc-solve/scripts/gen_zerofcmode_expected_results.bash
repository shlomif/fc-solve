#!/usr/bin/env bash
./summary-fc-solve seq 1 32000 -- --freecells-num 0 --tests-order 0AB -sp r:tf -mi 10000 | perl -lpE 's/; Length: \K[0-9]+/-1/' | tee ../scripts/ms32000_zerofc_mode_expected.txt
sha256sum ../scripts/ms32000_zerofc_mode_expected.txt | perl -lanE 'say $F[0]' > ../scripts/ms32000_zerofc_mode_expected.txt.sha256
