
import JsonConfigBase
import LocalConfigBase
import sys
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"].append( "ak5PFJetsL1" )

conf["InputFiles"] = "/afs/naf.desy.de/user/p/piparo/lustre/ZPJ2011/DrellYan_dy_fall10_weighted_[0-9]*.root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "mc_fall10_dy"

conf["UseWeighting"] = 1
conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,300], conf, True )

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

