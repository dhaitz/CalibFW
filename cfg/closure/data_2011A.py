
import sys
import ClosureConfigBase as cbase
import subprocess
import copy
import subprocess

conf = cbase.GetDataBaseConfig()
conf["InputFiles"] = cbase.CreateFileList( "/storage/5/hauth/zpj/Kappa_Run2011A/*.root")
#conf["InputFiles"] = cbase.CreateFileList( "/storage/6/zeise/events/ntuples/428/hltmu/kappa_2011-*DYmumu_Z2_powheg*32.root")


conf["OutputPath"] = "closure_data_2011A"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets" , "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res" ]
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = cbase.ExpandDefaultDataConfig(  algorithms, conf, True )

cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = True )
cbase.AddCutConsumer( conf , ["AK5PFJetsL1L2L3", "AK5PFJetsL1", "AK5PFJetsL1L2"] )
#cbase.AddHltConsumer( conf , base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5" ] )
cbase.AddLumiConsumer( conf , base_algorithms )


#JsonConfigBase.StoreSettings( conf, sys.argv[0] + ".json" )

cbase.Run( conf, sys.argv[0] + ".json")

subprocess.call("pprof --callgrind closure closure.prof > closure.callgrind", shell=True)

