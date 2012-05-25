#!/bin/bash
STARTM=`date -u "+%s"`

function fmerge {
DIR="$1"
OUT="${1}/temp"
mkdir $OUT -p
INC=10
INC="$2"
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
JOBS=0
for (( i=0; i<${tLen};)); do
    out="${OUT}/mergedfile_inc${INC}_${i}.root"
    ifiles=""
    for j in $(seq 1 $INC); do
        let iter=$i+$j-1
        ifiles="$ifiles ${fileArray[$iter]}"
    done

    # run the job
    echo -e "Job ${JOBS}: $out \c "
    echo "ROOTSYS=${ROOTPATH} PATH=$PATH:$ROOTSYS/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:$ROOTSYS/lib/root hadd ${out} ${ifiles}" | qsub -q short

    let i+=$INC
    let JOBS+=1
done
#echo "All jobs are submitted. Please use qstat to watch them."
}

############
### MAIN ###
############

OUT0="$1"

# start fmerge for the first time
fmerge "$1" "$2"

#check if there was more than 1 job = more than 1 output file...
while (($JOBS > 1)); do

    # NF = number of qstat output lines minus 2 -> number of unfinished jobs
    NF=`qstat | wc -l`
    let NF-=2
        
    #check if there are still jobs running
    while (($NF > 0)); do
        echo "Waiting for $NF jobs to finish..."
        sleep 5 
       
        NF=`qstat | wc -l`
        let NF-=2
    done
    
    #start fmerge again
    echo -e "\nRepeat merging process:"
    fmerge $OUT $INC

done


NF=`qstat | wc -l`
let NF-=2

while (($NF > 0)); do
    echo "Waiting for last job to finish..."
    sleep 3
     
    NF=`qstat | wc -l`
    let NF-=2
done


#copy closure file to original output dir and delete temp
echo "Creating closure file: "${OUT0}"/closure.root"
cp $out "${OUT0}/closure.root"
echo "Removing temp directory"
rm "${OUT0}/temp/" -r


#calculate running time
STOPM=`date -u "+%s"`
RUNTIMEM=`expr $STOPM - $STARTM`
if (($RUNTIMEM>59)); then
    TTIMEM=`printf "%dm%ds\n" $((RUNTIMEM/60%60)) $((RUNTIMEM%60))`
else
    TTIMEM=`printf "%ds\n" $((RUNTIMEM))`
fi


echo "Done!"
echo "Merging took: $TTIMEM"

