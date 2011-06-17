#! /bin/sh


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/lib:`pwd`/../Kappa/lib:`pwd`/../KappaTools/lib:/usr/lib/root/
export PATH=$PATH:`pwd`/bin:`pwd`/scripts
export CFGPATH=`pwd`/cfg
export FWPATH=`pwd`
export PYTHONPATH=$PYTHONPATH:$CFGPATH:$CFGPATH/Fragments:`pwd`:`pwd`/macros



