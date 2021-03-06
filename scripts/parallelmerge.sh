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
    #echo "ROOTSYS=${ROOTPATH} PATH=$PATH:$ROOTSYS/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:$ROOTSYS/lib/root hadd -f ${out} ${ifiles}" | qsub -q express -cwd
    echo "PATH=$PATH:/wlcg/sw/cms/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_4/external/slc5_amd64_gcc462/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/wlcg/sw/cms/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_3_4/external/slc5_amd64_gcc462/lib:/lib64:/wlcg/sw/cms/slc5_amd64_gcc462/external/gcc/4.6.2/lib64 hadd -f ${out} ${ifiles}" | qsub -q express -cwd

    let i+=$INC
    let JOBS+=1
done
}

############
### MAIN ###
############

OUT0="$1"
OUT="$1"
INC="$2"
JOBS=10000


#check if there was more than 1 job 
while (($JOBS > 1)); do

    #start fmerge
    echo -e "\nSubmitting jobs: "
    fmerge $OUT $INC
    sleep 2

    # NF = number of for jobs from STDIN retrieved from qstat
    NF=`qstat | grep STDIN |  wc -l`
        
    #check if there are still jobs running
    while (($NF > 0)); do
        echo "Waiting for $NF jobs to finish..."
        sleep 5 
       
        NF=`qstat | grep STDIN | wc -l`
    done

    if [ $JOBS = 1 ]; then
        break
    fi
        

done

#copy artus file to original output dir and delete temp
echo "Creating Artus output file: "${OUT0}"/artus.root"
cp $out "${OUT0}/artus.root"
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
