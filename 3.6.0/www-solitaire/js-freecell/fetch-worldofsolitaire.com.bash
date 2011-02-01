#!/bin/bash

# wget -np -N -r -X '/WorldOfSolitaireOffline.zip' 'http://worldofsolitaire.com/index-dev.html'
httrack 'worldofsolitaire.com/index-dev.html' -O "download" '-*.zip'
