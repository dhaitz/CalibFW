
import JsonConfigBase
import LocalConfigBase
import sys
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = [ "ak5PFJetsL1L2L3" ]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/piparo/Paper/ZPJ2011_DY_pu_corrected/*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "mc_fall10_dy_pu_flavour"

conf["UseWeighting"] = 0
conf["UseEventWeight"] = 0
conf["UseGlobalWeightBin"] = 0

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,20,30,50,80,110, 150, 200, 350], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")


