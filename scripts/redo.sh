#!/bin/bash

if [[ $1 != *"cfg"* ]]; then
  NAME=$1
  FILE="cfg/artus/${1}.py"
else
  NAME=${1##*/}
  NAME=${NAME%.*}
  FILE=$1
fi

echo "Work:" $CLOSURE_WORK
echo "Base:" $CLOSURE_BASE
echo "Name:" $NAME
echo "File:" $FILE

rm ${CLOSURE_WORK}/work/${NAME}"_old/" -r
mv ${CLOSURE_WORK}/work/${NAME}/ ${CLOSURE_WORK}/work/${NAME}_old/ -f

echo "Config file batch ..."
python ${CLOSURE_BASE}/${FILE} --batch

echo "Create output directory ..."
mkdir ${CLOSURE_WORK}/work/${NAME}/work.${NAME}/
cd ${CLOSURE_WORK}/work/${NAME}/

echo "Start grid control ..."
echo -en "\033]0;gc is running: ${NAME}\a"
/home/${USER}/git/grid-control/go.py  ${CLOSURE_WORK}/work/${NAME}/${NAME}.conf -cG

echo "Merge files ... "
hadd out/artus.root out/*.root

