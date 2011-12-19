#! /bin/sh

export CALIB_FW_BASE=`pwd`

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CALIB_FW_BASE:$CALIB_FW_BASE/../Kappa/lib:$CALIB_FW_BASE/../KappaTools/lib:$CALIB_FW_BASE/external/lib:/usr/lib/root/

#export PYTHONPATH=$PYTHONPATH:$CFGPATH:$CFGPATH/Fragments:`pwd`:`pwd`/macros
