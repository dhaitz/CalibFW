#!/bin/bash
zenity --notification --window-icon=data/excal_small.jpg
zenity --warning --text "Moechtest du wirklich excalibur benutzen\?" 

CFG=$(zenity --list --column "cfg" --text "Welche cfg willst du benutzen?"  "data" "data_ee" "mc" "mc_ee")
NFILES=$(zenity --scale 0 10 --text "Wie viele Dateien willst du verwenden?" --title "n files:")

artus $CFG -f $NFILES
