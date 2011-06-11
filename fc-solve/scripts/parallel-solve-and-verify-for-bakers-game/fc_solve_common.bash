#!/bin/bash
export FCS_PATH="$HOME/fc-solve-bakers-game-range"
export FCS_MAX_DEAL_IDX="10000" 
export FCS_PRESET_CONFIG="-ni -to 01ABCDEF -g bakers_game"
export FCS_MIN_DEAL_IDX="1" 
export FCS_DEALS_PER_BATCH="1000"

if test -z "$FCS_SOURCE_URL" ; then
    export FCS_SOURCE_URL="http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source"
fi
