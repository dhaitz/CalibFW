#!/bin/bash

if [[ $1 != *"cfg"* ]]; then
  NAME=$1
  FILE="cfg/artus/${1}.py"
else
  NAME=${1##*/}
  NAME=${NAME%.*}
  FILE=$1
fi

echo "Work:" $EXCALIBUR_WORK
echo "Base:" $EXCALIBUR_BASE
echo "Name:" $NAME
echo "File:" $FILE

rm ${EXCALIBUR_WORK}/work/${NAME}"_old/" -r
mv ${EXCALIBUR_WORK}/work/${NAME}/ ${EXCALIBUR_WORK}/work/${NAME}_old/ -f

echo "Config file batch ..."
python ${EXCALIBUR_BASE}/${FILE} --batch

echo "Create output directory ..."
mkdir ${EXCALIBUR_WORK}/work/${NAME}/work.${NAME}/
cd ${EXCALIBUR_WORK}/work/${NAME}/

echo "Start grid control ..."
echo -en "\033]0;gc is running: ${NAME}\a"
/home/${USER}/git/grid-control/go.py  ${EXCALIBUR_WORK}/work/${NAME}/${NAME}.conf -cG

echo "Merge files ... "
hadd out/artus.root out/*.root

