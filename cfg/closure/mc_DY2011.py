
import sys
import ClosureConfigBase as cbase
import subprocess
import copy

conf = cbase.GetMcBaseConfig()

conf["InputFiles"] = cbase.CreateFileList( "/storage/5/hauth/zpj/Kappa_DYToMuMu_Summer11-PU/*.root")

#conf["InputFiles"] = cbase.CreateFileList( "/storage/6/zeise/events/ntuples/428/hltmu/kappa_2011-10-27_MZ_428_HLTMu_DYmumu_Z2_pythia_0020_PU_S4_794.root")

#/storage/6/zeise/events/ntuples/428/hltmu/kappa_2011-10-27_MZ_428_HLTMu_DYmumu_Z2_pythia_0020_PU_S4_794.root

conf["OutputPath"] = "closure_mc_DY2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5PFJets" , "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3" ]
base_algorithms = ["AK5PFJets" ]

#[15,30,60,100,140,300]
conf = cbase.ExpandDefaultMcConfig(  algorithms, conf, True )

cbase.ApplyReweightingSummer11May10ReReco( conf )

jetpt_var = cbase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )



cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = False )
cbase.AddCutConsumer( conf , ["AK5PFJetsL1L2L3"] )

#cbase.StoreSettings( conf, sys.argv[0] + ".json" )

conf["Pipelines"] = dict( conf["Pipelines"].items() + jetpt_var.items() )

cbase.Run( conf, sys.argv[0] + ".json")

subprocess.call("pprof --callgrind closure closure.prof > closure.callgrind", shell=True)
