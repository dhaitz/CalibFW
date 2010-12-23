
import JsonConfigBase
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/local/scratch/hauth/data/ZPJ2010/skim_data/data*_job_Mu_4*.root" 
conf["OutputPath"] = "proto"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,300], conf )

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )


JsonConfigBase.StoreSettings( conf, "conf.json")
