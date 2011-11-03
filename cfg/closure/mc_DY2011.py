
import sys
import ClosureConfigBase as cbase
import LocalConfigBase
import subprocess
import copy

conf = cbase.GetMcBaseConfig()

conf["InputFiles"] = cbase.CreateFileList( "/home/poseidon/uni/data/Kappa/Kappa_DYToMuMu_Summer11-PU/*.root") 
conf["OutputPath"] = "closure_mc_DY2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets" , "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3" ] 
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = cbase.ExpandDefaultMcConfig( [15,30,60,100, 140,300], algorithms, conf, True )

cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = False )
cbase.AddCutConsumer( conf , ["AK5PFJets"] )

#cbase.StoreSettings( conf, sys.argv[0] + ".json" )

cbase.Run( conf, sys.argv[0] + ".json")

subprocess.call("pprof --callgrind closure closure.prof > closure.callgrind", shell=True) 
