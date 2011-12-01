
import sys
import ClosureConfigBase as cbase
import subprocess
import copy

conf = cbase.GetMcBaseConfig()
conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "Kappa_MC_428_new/*Summer11*.root"), sys.argv )

conf["OutputPath"] = "closure_mc_DY2011"

#algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3"   ]

algorithms = [ "AK5GenJets",
               "AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2", "AK5PFJetsL1L2L3",
               "AK5PFJetsCHS" , "AK5PFJetsCHSL1", "AK5PFJetsCHSL1L2", "AK5PFJetsCHSL1L2L3" ]
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

#[15,30,60,100,140,300]
conf = cbase.ExpandDefaultMcConfig(  algorithms, conf, True )

cbase.ApplyReweightingSummer11For2011A( conf )
#cbase.ApplyReweightingSummer11May10ReReco( conf )

jetpt_var = cbase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

cbase.AddCorrectionPlots( conf, base_algorithms, l3residual = False )
cbase.AddCutConsumer( conf , ["AK5PFJetsCHSL1L2L3", "AK5PFJetsL1L2L3"] )

#cbase.StoreSettings( conf, sys.argv[0] + ".json" )

conf["Pipelines"] = dict( conf["Pipelines"].items() + jetpt_var.items() )

cbase.Run( conf, sys.argv)

#subprocess.call("pprof --callgrind closure closure.prof > closure.callgrind", shell=True)
