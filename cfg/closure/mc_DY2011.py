

import sys
import ClosureConfigBase as cbase
import subprocess
import copy

conf = cbase.GetMcBaseConfig()
conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "Kappa_MC_428_14A/*pythia_Summer11*.root"), sys.argv )

conf["OutputPath"] = "closure_mc_DY2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ 
               "AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3",
               "AK5PFJetsCHS" , "AK5PFJetsCHSL1", "AK5PFJetsCHSL1L2", "AK5PFJetsCHSL1L2L3" ]
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]


# select just ONE event !!!		Run: 1 LumiSec: 1626 EventNum: 1120125
#pline_def = conf["Pipelines"]["default"]
# we can select just one event ! 
#pline_def["Filter"] += [ "runrange" ]
#pline_def["FilterRunRangeLow"] = 1
#pline_def["FilterRunRangeHigh"] = 1

#pline_def["FilterRunRangeLumiLow"] = 1626
#pline_def["FilterRunRangeLumiHigh"] = 1626

#pline_def["FilterRunRangeEventLow"] = 1120125
#pline_def["FilterRunRangeEventHigh"] = 1120125

conf = cbase.ExpandDefaultMcConfig(  algorithms, conf, True )

cbase.ApplyReweightingSummer11For2011A( conf )

#cbase.ApplyReweightingSummer11May10ReReco( conf )

# create various variations ... 
variations = []
variations += [ cbase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  ) ]
variations += [ cbase.ExpandRange2( variations[-1], 'Npv', [0, 3, 6, 12], [2, 5, 11, 100], "var_{name}_{low}to{high}", False) ]

cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = False )
cbase.AddCutConsumer( conf , ["AK5PFJetsCHSL1L2L3", "AK5PFJetsL1L2L3"] )

#merge variations
for v in variations:
    conf["Pipelines"] = dict( conf["Pipelines"].items() + v.items() )

cbase.Run( conf, sys.argv )

