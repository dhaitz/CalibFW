
import JsonConfigBase
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["InputFiles"] = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/skim_zjet_fall10/*.root" 
conf["OutputPath"] = "proto_mc"

conf["UseWeighting"] = 1
conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,300], conf )

muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )


JsonConfigBase.StoreSettings( conf, "process_mc.json")
