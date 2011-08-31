
import JsonConfigBase
import LocalConfigBase
import sys
import copy
conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3"] #, "ak7PFJetsL1", "ak7PFJetsL1L2L3"  ]

conf["InputFiles"] = "/home/poseidon/uni/data/ZPJ2011/DYToMuMu_Summer11-PU/*.root"
conf["OutputPath"] = "DYToMuMu_Summer11-PU-comp"

conf["UseWeighting"] = 0
#conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,45,60,80,100, 120, 140, 180, 300, 1000], conf, True )

JsonConfigBase.ApplyReweightingSummer11May10ReReco( conf )


# additional consumer
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
