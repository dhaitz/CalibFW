import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = ["ak5PFJets", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3CHS"]
conf["InputFiles"] = "/data/berger/data/Run2011A/*.root"
conf["OutputPath"] = "/data/berger/data/data_Oct19"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000], conf, True )


back2back_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  )
secjet_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items())
conf["Pipelines"] = dict( conf["Pipelines"].items() + JsonConfigBase.CreateEndcapPipelines( conf["Pipelines"] ).items())

conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
