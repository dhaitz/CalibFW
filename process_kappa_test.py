
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["InputFiles"] = JsonConfigBase.CreateFileList( "/home/poseidon/uni/data/ZPJ2011/Kappa_DYToMuMu_Summer11-PU/*.root") 
conf["OutputPath"] = "kappa_data_2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3" ] 
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = JsonConfigBase.ExpandDefaultMcConfig( [15,30,60,100, 140,300], algorithms, conf, True )



JsonConfigBase.AddCorrectionPlots( conf, base_algorithms )
JsonConfigBase.AddCutConsumer( conf , ["AK5PFJetsL1L2L3", "AK5PFJetsL1", "AK5PFJetsL1L2"] )


JsonConfigBase.ApplyReweightingSummer11May10ReReco( conf )


#JsonConfigBase.StoreSettings( conf, sys.argv[0] + ".json" )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

