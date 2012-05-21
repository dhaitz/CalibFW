#!/bin/bash
DIR="$1"
OUT="$2"
INC=10
INC="$3"
ROOTPATH=/wlcg/sw/cms/slc5_amd64_gcc462/lcg/root/5.32.00

# failsafe - fall back to current directory
[ "$DIR" == "" ] && DIR="."

# save and change IFS
OLDIFS=$IFS
IFS=$'\n'

# read all file name into an array
fileArray=($(find $DIR -type f))

# restore it
IFS=$OLDIFS

# get length of an array
tLen=${#fileArray[@]}
inputFiles=()

# use for loop read all filenames
for (( i=0; i<${tLen};)); do
    out="${OUT}/mergedfile_inc${INC}_${i}.root"
    ifiles=""
    for j in $(seq 1 $INC); do
        let iter=$i+$j-1
        ifiles="$ifiles ${fileArray[$iter]}"
    done

    # run the job
    echo -e "Job ${i}: $out \c "
    echo "ROOTSYS=${ROOTPATH} PATH=$PATH:$ROOTSYS/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:$ROOTSYS/lib/root hadd ${out} ${ifiles}" | qsub -q short

    let i+=$INC
done
echo "All jobs are submitted. Please use qstat to watch them."