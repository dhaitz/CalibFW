
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3Res", "ak7PFJetsL1L2L3Res"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/piparo/DiplomArbeitThomas/ZPJ2011_data_nov4_newjetid/data_2010*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/data2010_v7_double"

#conf["Pipelines"]["default"]["CutSecondLeadingToZPt"] = 0.3

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
