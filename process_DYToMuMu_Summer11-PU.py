
import JsonConfigBase
import LocalConfigBase
import sys
import copy
conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak7PFJets", "ak7PFJetsL1L2L3", "ak5PFJets", "ak5PFJetsL1L2L3"]

conf["InputFiles"] = "/home/poseidon/uni/data/ZPJ2011/DYToMuMu_Summer11-PU/*.root"
conf["OutputPath"] = "DYToMuMu_Summer11-PU-neu"

conf["UseWeighting"] = 0
#conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,1000], conf, True )

JsonConfigBase.ApplyReweightingSummer11May10ReReco( conf )

back2back_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
secjet_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


#conf["Pipelines"]["default"]["CutSecondLeadingToZPtJet2Threshold"] = 0

#SecondJetNoCut = copy.deepcopy( conf["Pipelines"]["default"] )
#SecondJetNoCut["FilterInCutIgnored"] = 16
#SecondJetNoCut["RootFileFolder"] = "NoSecondJetCut"

#conf["Pipelines"]["NoSecondJetCut"] = SecondJetNoCut

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", range(10, 20), True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


conf["Pipelines"] = dict( conf["Pipelines"].items() + back2back_var.items() + secjet_var.items() )




#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )

# additional consumer
#conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
