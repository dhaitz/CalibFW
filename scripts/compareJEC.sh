#!/bin/bash
# usage: compareJEC Tag1 Tag2
[ "$1" == "" ] && echo "Please provide two correction tags!" && exit
[ "$2" == "" ] && echo "Please provide a second correction tag!" && exit

for a in AK5PF AK5PFchs
do
  for l in L1FastJet L1Offset L2Relative L3Absolute L2L3Residual L5Flavor L5Flavor_bJ L5Flavor_cJ L5Flavor_gJ L5Flavor_qJ Uncertainty
  do
    #echo -e "$l $a :"
    printf "%12s %9s: " $l $a
    diff -qw {${1},${2}}_${l}_${a}.txt
    if [[ $? == 0 ]]; then echo ""; fi
  done
done
