#!/bin/bash

export CLOSURE_BASE=$(readlink -e .)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CLOSURE_BASE:$CLOSURE_BASE/../Kappa/lib:$CLOSURE_BASE/../KappaTools/lib:$CLOSURE_BASE/external/lib:/usr/lib/root/
