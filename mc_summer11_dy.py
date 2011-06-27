import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3NoPU"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/hauth/ZPJ2011/DYToMuMu_Summer11-PU/DYToMuMu_Summer11-PU_DYToMuMu_M-20_TuneZ2_7TeV-pythia6_*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/chs_Summer11_mc_withrw"

JsonConfigBase.ApplyReweightingSummer11May10ReReco(conf)

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
