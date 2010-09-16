#! /bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/lib
export PATH=$PATH:`pwd`/bin:`pwd`/scripts
export CFGPATH=`pwd`/cfg
export PYTHONPATH=$PYTHONPATH:$CFGPATH:$CFGPATH/Fragments

