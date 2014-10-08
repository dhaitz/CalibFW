#!/bin/bash

help()
{
    echo "help";
    echo "    -c continue";
    echo "    -s site (gridka or desy)";
    echo "    -d remote srm dir";
    echo "    -p path to your local storage directory";
    echo "    -h help";
}

if [[ $HOSTNAME == *naf* ]]; then
    export STORAGEPATH="/nfs/dust/cms/user/${USER}/skims"
else
    export STORAGEPATH="/storage/9/${USER}/skims"
fi
export MYSEDIR="2014_07_18_ee-mm-mcRD"
CONTINUE=false;
export GRIDSITE="gridka"

while getopts hcd:s:p: opt
do
   case $opt in
       c) CONTINUE=true;;
       d) export MYSEDIR=($OPTARG);;
       s) export GRIDSITE=($OPTARG);;
       p) export STORAGEPATH=($OPTARG);;
       h) help; exit 0;;
   esac
done

if [ $GRIDSITE == 'gridka' ]; then
    export MYSEPATH="srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/users/${USER}"
else
    export MYSEPATH="srm://dcache-se-cms.desy.de/pnfs/desy.de/cms/tier2/store/user/${USER}"
fi

if [ $CONTINUE = false ]; then
    # overwrite txt file, if exists
    if [ -f ${MYSEDIR}__files.txt ]; then
        rm -i ${MYSEDIR}__files.txt
    fi

    if [ -f ${MYSEDIR}__files-sorted.txt ]; then
        rm -i ${MYSEDIR}__files-sorted.txt
    fi


    # get file list, save in txt file
    for i in `seq 0 900 3600`
        do `srmls -2 --streams_num=1  -count=900 -offset=${i} ${MYSEPATH}/${MYSEDIR}| grep root >>${MYSEDIR}__files.txt 2>&1`
    done

    if [ -f ${MYSEDIR}__files.txt ]; then
        echo "file list saved in ${MYSEDIR}__files.txt Number of files:"
        less ${MYSEDIR}__files.txt | wc -l
    fi

    # created sorted version
    less ${MYSEDIR}__files.txt | cut -d "/" -f 9 | sort > ${MYSEDIR}__files-sorted.txt

    mkdir -p ${STORAGEPATH}/${MYSEDIR}
fi

echo "Starting download from ${MYSEPATH}/${MYSEDIR} to ${STORAGEPATH}/${MYSEDIR}"
# loop over files, download
less ${MYSEDIR}__files-sorted.txt | while read p; do
    if [ ! -f ${STORAGEPATH}/${MYSEDIR}/${p} ]; then
        echo -e "Downloading ${p} ... \c"
        srmcp -2 --streams_num=1 ${MYSEPATH}/${MYSEDIR}/${p} file:///${STORAGEPATH}/${MYSEDIR}/${p}
        echo "done!"
    fi
done

echo "downloaded to ${STORAGEPATH}/${MYSEDIR}  Number of files:"
ls -l ${STORAGEPATH}/${MYSEDIR}/*.root | wc -l

