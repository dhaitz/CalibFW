
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/home/poseidon/uni/data/ZPJ2011/Run2011A/*.root" 
conf["OutputPath"] = "Run2011A-combined-comp"

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,45,60,80,100, 120, 140, 180, 300, 1000], conf, True )

conf["Algos"] = ["ak5PFJetsL1L2L3Res" ]#, "ak7PFJetsL1", 
#"ak7PFJetsL1L2L3","ak7PFJetsL1L2L3Res"]


conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]
#conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items() + endcap.items() )# + back2back_var.items() )



#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )
JsonConfigBase.Run( conf, sys.argv[0] + ".json")
