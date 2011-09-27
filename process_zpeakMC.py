import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3CHS"]
conf["InputFiles"] = "/data/berger/data/DYToMuMu-Summer11/*.root"
conf["OutputPath"] = "/data/berger/data/MC_Zmumu"

conf["UseWeighting"] = 0
conf["Pipelines"]["default"]["CutLeadingJetEta"] = 5.0
conf["Pipelines"]["default"]["CutBack2Back"] = 10.0
conf["Pipelines"]["default"]["CutSecondLeadingToZPt"] = 1.0

conf = JsonConfigBase.ExpandDefaultMcConfig( [0, 30, 45, 60, 80, 105, 140, 1000], conf, True )

JsonConfigBase.ApplyReweightingSummer11May10ReReco( conf )

conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
