#!/bin/bash
dir=$(pwd)
if [ ${dir##*/} != "CalibFW" ]; then
  echo "You must be in the CalibFW directory!" 
  exit
fi

afspath=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV
jsondestination=data/json
pudestination=data/pileup
variant="PromptReco_Collisions12_JSON"

if [ ${#1} -gt 2 ]; then
  variant=$1
fi

afsjson=$(ls $afspath/Prompt/Cert_*_${variant}{_v*.txt,.txt} | tail -n 1)
locjson=${jsondestination}/${afsjson##*/}
afspu=$(ls $afspath/PileUp/pileup_JSON_*.txt | tail -n 1)
locpu=${pudestination}/${afspu##*/}
cmd="macros/weightCalc.py -l $locpu $locjson /path/to/skim/*.root"

check_json() {
  if [ ! -e $2 ]; then
    # this is the important line: copying from afspath to destination:
    cp -i $1 $2
    echo "${1##*/} copied successfully."
    echo "  You can recalculate the pu weights with:"
    echo "  $3"
  else
    e=$(diff $1 $2 | wc -l)
    if [ $e -gt 0 ]
    then
       echo "You already have the same file, but they differ:"
       echo "diff $1 $2"
       diff $1 $2
       echo "Do you want to overwrite your version? [y/N]"
       read answer
       if [ "$answer" == "y" ]; then
         cp $1 $2
         echo "$(ls $2) copied successfully."
         echo "  You can recalculate the pu weights with:"
         echo "  $3"
       fi
    else
       diff $1 $2
       echo "You already downloaded the newest json."
    fi
  fi
}

echo -e "JSON:   \c"
check_json $afsjson $locjson "$cmd"
echo -e "Pileup: \c"
check_json $afspu $locpu "$cmd"
