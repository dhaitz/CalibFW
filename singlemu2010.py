import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3Res", "ak7PFJetsL1L2L3Res"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/piparo/DiplomArbeitThomas/ZPJ2011_data_nov4_newjetid/data_2010*_out.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/data2010_v7_single"

#conf["Pipelines"]["default"]["CutSecondLeadingToZPt"] = 0.3
conf["Pipelines"]["default"]["CutHLT"] = "SingleMu"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#secjet_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.17, 0.2,0.25, 0.3], True, True  )
#jeteta_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutLeadingJetEta", [1.3, 1.8, 5.0], True, True  )


#conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items() )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
