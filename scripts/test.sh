#!/bin/bash
# usage: scripts/test.sh commit|branch
# Be careful, commits are checked out and the code is recompiled!
testconfig=cfg/artus/data_2012.py

git checkout $1
make $2 -j5
echo
python $testconfig --fast 20 24
output=tst_$(git describe --long)
output=${output/-/.}
output=${output/-g/-}
cp ${testconfig}.json ${output}.json
testconfig=${testconfig/*\//}
cp artus_${testconfig/\.py/}.log ${output}.log
echo "artus config and logfile saved to: $output.json and $output.log"
