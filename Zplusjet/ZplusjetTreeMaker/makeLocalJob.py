#!/usr/bin/env python
#
import getopt, sys, os

filename = 'zplusjettreemaker_generalisation_cfg.py'
inFile = open(filename,'r').readlines()
outFile = open('local_'+filename,'w')

        
for line in inFile:
    line = line.replace("__SKIP_EVENTS__",str(0))
    line = line.replace("__MAX_EVENTS__",str(10))
    line = line.replace("__FILE_NAMES__",str("'file:/storage/1/vbuege/Beispiele_Rootfiles/Summer09/store/relval/CMSSW_3_1_0/RelValZMM/GEN-SIM-RECO/STARTUP31X_V1-v1/0001/D22D3E9C-8966-DE11-900A-001617C3B66C.root'"))
    outFile.write(line)
outFile.close()
    

    


