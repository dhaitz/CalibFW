import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJets", "ak5PFJetsL1", "ak5PFJetsL1CHS", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3CHS"] #, "ak7PFJetsL1", "ak7PFJetsL1L2L3"  ]
conf["InputFiles"] = "/data/berger/data/DYToMuMu-Summer11/*.root"
conf["OutputPath"] = "/data/berger/data/MC_Sept08"

conf["UseWeighting"] = 0
conf = JsonConfigBase.ExpandDefaultMcConfig( [0, 30, 45, 60, 80, 105, 140, 1000], conf, True )

JsonConfigBase.ApplyReweightingSummer11May10ReReco( conf )

back2back_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
secjet_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items())
conf["Pipelines"] = dict( conf["Pipelines"].items() + JsonConfigBase.CreateEndcapPipelines( conf["Pipelines"] ).items())



#conf["Pipelines"]["default"]["CutSecondLeadingToZPtJet2Threshold"] = 0

#SecondJetNoCut = copy.deepcopy( conf["Pipelines"]["default"] )
#SecondJetNoCut["FilterInCutIgnored"] = 16
#SecondJetNoCut["RootFileFolder"] = "NoSecondJetCut"

#conf["Pipelines"]["NoSecondJetCut"] = SecondJetNoCut

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", range(10, 20), True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

#conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items() ) # back2back_var.items() + 
#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )

# additional consumer
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
