import sys
import ClosureConfigBase as cbase
import subprocess
import copy

conf = cbase.GetMcBaseConfig()
conf["InputFiles"] = cbase.ApplyFast( cbase.CreateFileList( cbase.GetDataPath() + "kappa44_MC11/*.root"), sys.argv )

conf["OutputPath"] = "closure_mc_DY2011_2ndjettest"

algorithms = [ "AK5PFJets", "AK5PFJetsL1L2L3", "AK5PFJetsCHS" , "AK5PFJetsCHSL1L2L3" ]
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf["Pipelines"]["default"]["Cuts"] += ['zpt']
conf["Pipelines"]["default"]["CutZPt"] = 20.0

conf = cbase.ExpandDefaultMcConfig(algorithms, conf, True )

cbase.ApplyReweightingFall11Powheg44ReReco( conf )

#This line is new:
cbase.Apply2ndJetReweighting( conf )

cbase.AddCutConsumer( conf , [ "AK5PFJetsCHSL1L2L3", "AK5PFJetsL1L2L3" ] )

cbase.Run( conf, sys.argv )

