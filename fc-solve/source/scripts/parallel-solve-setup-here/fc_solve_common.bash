#!/bin/bash
export FCS_PATH="$HOME/fc-solve-bakers-game-range"
export FCS_MAX_DEAL_IDX="4000" FCS_PRESET_CONFIG="-g bakers_game" 
export FCS_MIN_DEAL_IDX="1" 
export FCS_DEALS_PER_BATCH="1"

if test -z "$FCS_SOURCE_URL" ; then
    export FCS_SOURCE_URL="http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source"
fi
