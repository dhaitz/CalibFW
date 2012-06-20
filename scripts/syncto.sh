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
CALIB=$CALIB/cfg/SkimmingCalibFW

ID=0
mydiff () {
  diff $1 $2
  ID+=$(diff $1 $2 | wc -l)
}

echo "Diffs between $CALIB and $1"
mydiff $1/src/Kappa/Producers/python/skim_Base_52x_ZJet.py $CALIB/skim_Base_52x_ZJet.py
mydiff $1/skim_Data_52x_ZJet.py $CALIB/skim_Data_52x_ZJet.py
mydiff $1/skim_MC_52x_ZJet.py $CALIB/skim_MC_52x_ZJet.py
mydiff $1/2012-06-15_JB_525_Data.conf $CALIB/2012-06-15_JB_525_Data.conf
mydiff $1/2012-06-15_JB_525_MC.conf $CALIB/2012-06-15_JB_525_MC.conf

if [ "$ID" != "000000" ]; then
  echo "You have now time to stop the sync..."
  read TEMP
fi

echo "Move latest configs from $CALIB to $1"
rsync -v $2 $CALIB/skim_Base_52x_ZJet.py $1/src/Kappa/Producers/python/
rsync -v $2 $CALIB/skim_Data_52x_ZJet.py $1
rsync -v $2 $CALIB/skim_MC_52x_ZJet.py $1
rsync -v $2 $CALIB/2012-06*525*.conf $1
