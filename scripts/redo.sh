#!/bin/bash

# DH:	For now this is useful only for me ... make this more generic
#	get data / work directory paths from ClosureConfigBase or implement directly

NAME="${1}"

echo "File: ${NAME}.py"
rm /storage/8/dhaitz/CalibFW/work/${NAME}"_old/" -r
mv /storage/8/dhaitz/CalibFW/work/${NAME}/ /storage/8/dhaitz/CalibFW/work/${NAME}_old/ -f

echo "Config file batch ..."
python /home/dhaitz/git/CalibFW/cfg/closure/${NAME}.py batch

echo "Create output directory ..."
mkdir /storage/8/dhaitz/CalibFW/work/${NAME}/work.${NAME}/
cd /storage/8/dhaitz/CalibFW/work/${NAME}/

echo "Start grid control ..."
/home/dhaitz/git/grid-control/go.py  /storage/8/dhaitz/CalibFW/work/${NAME}/${NAME}.conf -cG

echo "Merge files ... "
. parallelmerge.sh



