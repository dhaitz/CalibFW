import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJets", "ak5PFJetsNoPU", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3NoPU"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/berger/zjet/DYToMuMu_Summer11-PU-A/*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/chs_Summer11_mc_withoutrw"
conf["UseWeighting"] = 1
conf["UseEventWeight"] = 1
conf["UseGlobalWeightBin"] = 1
conf["GlobalXSection"] = 1614.0
JsonConfigBase.ApplyReweightingSummer11May10ReReco(conf)

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
