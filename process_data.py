
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = LocalConfigBase.GetLocalDataPath() + "skim_data/data*_job_Mu_*.root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() +"proto"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,300], conf, True )

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )
JsonConfigBase.Run( conf, sys.argv[0] + ".json")
