#!/bin/bash

export BOOSTPATH=/wlcg/sw/boost/current/
export CLOSURE_BASE=$(readlink -e .)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CLOSURE_BASE:$CLOSURE_BASE/../Kappa/lib:$CLOSURE_BASE/../KappaTools/lib:$BOOSTPATH/lib

# Set some user specific variables
if [ $USER = "dhaitz" ]; then
    if [ -f /storage/a/dhaitz/CalibFW/work/data/out/closure.root ]; then
        export DATA='/storage/a/dhaitz/CalibFW/work/data/out/closure.root'
    else
        export DATA='/storage/a/dhaitz/CalibFW/work/data_old/out/closure.root'
    fi
    if [ -f /storage/a/dhaitz/CalibFW/work/mc/out/closure.root ]; then
        export MC='/storage/a/dhaitz/CalibFW/work/mc/out/closure.root'
    else
        export MC='/storage/a/dhaitz/CalibFW/work/mc_old/out/closure.root'
    fi
    export USERPC="ekplx26"
elif [ $USER = "berger" ]; then
    export DATA='/storage/a/dhaitz/CalibFW/work/data/out/closure.root'
    export MC='/storage/a/dhaitz/CalibFW/work/mc/out/closure.root'
    export USERPC="ekplx45"
fi


alias plotsync='rsync $CLOSURE_BASE/out/plot.pdf $USER@$USERPC.physik.uni-karlsruhe.de:/usr/users/$USER/plot.pdf'

alias plot='python $CLOSURE_BASE/plotting/plotbase.py'
alias plott='python $CLOSURE_BASE/plotting/plotbase.py $DATA $MC'
alias plotfile='python $CLOSURE_BASE/plotting/plotbase.py $DATA $MC -o $CLOSURE_BASE/out/ -f pdf --filename plot'
alias plotfilemc='python $CLOSURE_BASE/plotting/plotbase.py $MC -o $CLOSURE_BASE/out/ -l 0 -f pdf --filename plot'

alias kc='killall closure'


# Usage 'fast' for testing of cfg files, e.g. 'fast data 3'
fast()
{
    python $CLOSURE_BASE/cfg/closure/${1}.py --fast $2
}


# Use this in combination with plotfile for live plotting
plotinit()
{
 ssh $USER@$USERPC "DISPLAY=:0 evince /usr/users/$USER/plot.pdf &" &
 export PROMPT_COMMAND="plotsync"
}


# Use this to open a root file directly in the TBrowser
rot()
{
    root -l $@ /home/dhaitz/tbrowser.cxx
}

# Use this to open the default data and mc files directly in a TBrowser
alias rooot='rot $DATA $MC'

