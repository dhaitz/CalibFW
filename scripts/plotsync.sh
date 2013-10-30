#!/bin/bash

# Variables and script needed for www plot sync
export DATE=`date +%Y_%m_%d`
if [ -d ${SYNCDIR} ]; then
    echo "Copying plots to webspace..."
    rsync ${SYNCDIR}/*.* $USER@$USERPC:/disks/ekpwww/web/$USER/public_html/plots_archive/${DATE}/
    echo "at URL:"
    for i in `ls ${SYNCDIR}/*.*`;do j=`basename $i`; echo http://www-ekp.physik.uni-karlsruhe.de/~$USER/plots_archive/${DATE}/${j}; done
    rm -r ${SYNCDIR}
else
    echo "Nothing to push"
fi
