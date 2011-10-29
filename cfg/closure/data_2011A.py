
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy
import subprocess

conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = JsonConfigBase.CreateFileList( "/home/poseidon/uni/data/Kappa/Kappa_Run2011A/skim.root") 
conf["OutputPath"] = "kappa_data_2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res" ] 
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = JsonConfigBase.ExpandDefaultDataConfig( [15,30,60,100, 140,300], algorithms, conf, True )

JsonConfigBase.AddCorrectionPlots( conf, base_algorithms, l3residual = True )
JsonConfigBase.AddCutConsumer( conf , ["AK5PFJetsL1L2L3", "AK5PFJetsL1", "AK5PFJetsL1L2"] )
JsonConfigBase.AddHltConsumer( conf , base_algorithms, ["HLT_Mu13_Mu8", "HLT_DoubleMu7" ] )
JsonConfigBase.AddLumiConsumer( conf , base_algorithms )


#JsonConfigBase.StoreSettings( conf, sys.argv[0] + ".json" )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

subprocess.call("pprof --callgrind resp_cuts resp_cuts.prof > resp_cuts.callgrind", shell=True) 
