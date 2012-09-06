#!/bin/bash
# This script calculates the number of events passing all cuts. This is only a
# quick and dirty solution which works only for certain configurations with
# filters.

# Settings
ALGO=AK5PFJetsCHSL1L2L3Res
LASTCUT=jet_mass

SUM=0
for i in ${1}/output/job_*/job.stdout*
do 
	if [ -e ${i/stdout*/stdout.gz} ]; then gzip -d ${i/stdout*/stdout.gz}; fi
	SUM=$(($(grep -6 $ALGO ${i/stdout*/stdout} | grep $LASTCUT | awk '{print $4}') + $SUM))
done
echo $SUM
