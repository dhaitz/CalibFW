#!/bin/bash
CALIB=$(pwd)
CALIB=${CALIB/CalibFW*/CalibFW}
if [ "CalibFW" != ${CALIB/*CalibFW/CalibFW} ] ; then
  echo "This script must be run within CalibFW!"
  exit
fi
if [ "$1" == "" ] ; then
  echo "Usage: syncto.sh /path/to/CMSSW_X_Y_Z"
  exit
fi
if [ "CMSSW" != ${1/*CMSSW*/CMSSW} ] ; then
  echo "Usage: syncto.sh /path/to/CMSSW_X_Y_Z"
  exit
fi
CALIB=$CALIB/cfg/cmssw

VERSION=${1/*CMSSW_/}
VERSION=${VERSION/_/}
VERSION=${VERSION/\//}
V1=${VERSION/_*/}
V2=${VERSION/_/}
echo "CMSSW $V2"

ID=0
mydiff () {
  diff $1 $2
  ID+=$(diff $1 $2 | wc -l)
}

echo "Diffs between $CALIB and $1"
mydiff "$1/src/Kappa/Producers/python/skim_Base_${V1}x_ZJet.py" "$CALIB/skim_Base_${V1}x_ZJet.py"
mydiff "$1/skim_Data_${V1}x_ZJet.py" "$CALIB/skim_Data_${V1}x_ZJet.py"
mydiff "$1/skim_MC_${V1}x_ZJet.py" "$CALIB/skim_MC_${V1}x_ZJet.py"
mydiff "$1/2012-*_${V2}_Data.conf" "$CALIB/2012-*_${V2}_Data.conf"
mydiff "$1/2012-*_${V2}_MC.conf" "$CALIB/2012-*_${V2}_MC.conf"

if [ "$ID" != "000000" ]; then
  echo "You have now time to stop the sync..."
  read TEMP
fi

echo "Move latest configs from $CALIB to $1"
rsync -v $2 $CALIB/skim_Base_${V1}x_ZJet.py $1/src/Kappa/Producers/python/
rsync -v $2 $CALIB/skim_Data_${V1}x_ZJet.py $1
rsync -v $2 $CALIB/skim_MC_${V1}x_ZJet.py $1
rsync -v $2 $CALIB/2012-*${V2}*.conf $1
