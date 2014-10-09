#!/bin/bash

zenity --warning --text "Moechtest du wirklich einen plot machen?" 

PLOT=$(zenity --list --column "plot" --text "Welchen plot willst du machen?" "zmass" "zpt")
NBINS=$(zenity --scale 0 100 --text "Wie viele Bins willst du verwenden?" --title "n files:")
FIT=$(zenity --list --column "fit" --text "Welchen fit willst du machen?"  "breitwigner" "gauss")

if [ $FIT == 'breitwigner' ]; then
   export FIT="bw"
fi

merlin -NL $PLOT --nbins $NBINS -F $FIT
