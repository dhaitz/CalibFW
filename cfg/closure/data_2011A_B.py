
import sys
import ClosureConfigBase as cbase
import subprocess
import copy
import subprocess

conf = cbase.GetDataBaseConfig()
conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "Kappa_Data11/*.root"), sys.argv )
#conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "CalibFW/cfg/SkimmingCalibFW/skim.root"), sys.argv )
#conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "Kappa_Run2011A/*.root"), sys.argv )


conf["OutputPath"] = "closure_data_2011A_B"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets" , "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res",
               "AK5PFJetsCHS" , "AK5PFJetsCHSL1", "AK5PFJetsCHSL1L2", "AK5PFJetsCHSL1L2L3", "AK5PFJetsCHSL1L2L3Res" ]
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

#[15,30,60,100,140,300]
conf = cbase.ExpandDefaultDataConfig(  algorithms, conf, True )

# create various variations ...
variations = []
variations += [ cbase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  ) ]
variations += [ cbase.ExpandRange2( variations[-1], 'Npv', [0, 3, 6, 12], [2, 5, 11, 100], "var_{name}_{low}to{high}", False) ]


cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = True )
cbase.AddCutConsumer( conf , [ "AK5PFJetsCHSL1L2L3", "AK5PFJetsCHSL1L2L3Res", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res" ] )
cbase.AddHltConsumer( conf , base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5" ] )
cbase.AddLumiConsumer( conf , base_algorithms )


#merge variations
for v in variations:
    conf["Pipelines"] = dict( conf["Pipelines"].items() + v.items() )


cbase.Run( conf, sys.argv )
