#! /bin/sh

if  [ $USER == "berger" ]; then
    export CALIB_FW_BASE=/portal/ekpcms5/home/berger/CalibFW
elif [ $USER == "hauth" ]; then
    export CALIB_FW_BASE=/storage/5/hauth/zpj/CalibFW
elif [ $USER == "dhaitz" ]; then
    export CALIB_FW_BASE=/portal/ekpcms5/home/dhaitz/git/CalibFW
else
    export CALIB_FW_BASE=`pwd`
    echo "CALIB_FW_BASE was set to $CALIB_FW_BASE. Please make sure that this is correct."
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CALIB_FW_BASE:$CALIB_FW_BASE/../Kappa/lib:$CALIB_FW_BASE/../KappaTools/lib:$CALIB_FW_BASE/external/lib:/usr/lib/root/
