
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = ["ak5PFJets", "ak5PFJetsNoPU", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3NoPU"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/berger/zjet/Run2011A-May10ReReco-A/*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/data2011_nopu"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
