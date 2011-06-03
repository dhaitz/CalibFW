
import JsonConfigBase
import LocalConfigBase
import sys
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = [ "ak5PFJets" ]

conf["InputFiles"] = "/home/poseidon/uni/data/ZPJ2011_mc/mc_414_berger_2011-05-09_DY_Spring11_powheg_0.root"
conf["OutputPath"] = "resp_cuts_test"

conf["UseWeighting"] = 0
#conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,300], conf, True )

#conf["Pipelines"]["default"]["CutSecondLeadingToZPtJet2Threshold"] = 0

#SecondJetNoCut = copy.deepcopy( conf["Pipelines"]["default"] )
#SecondJetNoCut["FilterInCutIgnored"] = 16
#SecondJetNoCut["RootFileFolder"] = "NoSecondJetCut"

#conf["Pipelines"]["NoSecondJetCut"] = SecondJetNoCut

muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", range(10, 20), True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )




#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )

# additional consumer
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

