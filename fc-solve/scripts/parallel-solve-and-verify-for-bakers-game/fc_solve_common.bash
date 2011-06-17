#!/bin/bash
export FCS_PATH="/tmp/fc-solve-bakers-game-range"
max_deal_idx="10,000,000"
export FCS_MAX_DEAL_IDX="${max_deal_idx//,/}"
export FCS_PRESET_CONFIG="-g bakers_game"
export FCS_SOLVER_THEME_CONFIG="-ni -to 01ABCDEF"
export FCS_MIN_DEAL_IDX="1" 
export FCS_DEALS_PER_BATCH="1000"

if test -z "$FCS_SOURCE_URL" ; then
    export FCS_SOURCE_URL="http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source"
fi
