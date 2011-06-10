#!/bin/bash
bash Setup.bash
tmux new -s fc_solve -n fcs 'echo ./queue && ./queue' \; \
    splitw -v -p 33 -t fcs.0 'sleep 0.02 && ./solve' \; \
    splitw -v -p 33 -t fcs.0 'sleep 1 && ./verify' \;
