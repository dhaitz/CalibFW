
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3Res", "ak7PFJetsL1L2L3Res"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/berger/skim2011_v6/skim_414_berger_2011-04-29_DoubleMu_2011A*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/data2011_v7_double"

#conf["Pipelines"]["default"]["CutSecondLeadingToZPt"] = 0.3

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
