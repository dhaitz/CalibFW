
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["InputFiles"] = JsonConfigBase.CreateFileList( "/home/poseidon/uni/data/Kappa/Kappa_DYToMuMu_Summer11-PU/*_0.root") 
conf["OutputPath"] = "kappa_mc_2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets" , "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3" ] 
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = JsonConfigBase.ExpandDefaultMcConfig( [15,30,60,100, 140,300], algorithms, conf, True )

JsonConfigBase.AddCorrectionPlots( conf, base_algorithms, l3residual = False )
JsonConfigBase.AddCutConsumer( conf , ["AK5PFJets"] )


#JsonConfigBase.StoreSettings( conf, sys.argv[0] + ".json" )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

