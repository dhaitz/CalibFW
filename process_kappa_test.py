
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["InputFiles"] = JsonConfigBase.CreateFileList( "/home/poseidon/uni/data/ZPJ2011/Kappa_DYToMuMu_Summer11-PU/*.root") 
conf["OutputPath"] = "kappa_data_2011"


conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,300], conf, True )

conf["Algos"] = ["ak5PFJetsL1L2"]


#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

#pline_only2ndJet = copy.deepcopy( conf["Pipelines"]["default"] ) 

#pline_only2ndJet["Filter"].append("cutselection")
#pline_only2ndJet["Filter2ndJetPtCutSet"] = True
#pline_only2ndJet["FilterDeltaPhiCutSet"] = False
#pline_only2ndJet["RootFileFolder"] = "cut_only2ndJet"
#pline_only2ndJet["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi

#pline_bothcuts = copy.deepcopy( conf["Pipelines"]["default"] ) 

#pline_bothcuts["Filter"].append("cutselection")
#pline_bothcuts["Filter2ndJetPtCutSet"] = True
#pline_bothcuts["FilterDeltaPhiCutSet"] = True
#pline_bothcuts["RootFileFolder"] = "cut_bothtopo"
#pline_bothcuts["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi

#pline_noBalance = copy.deepcopy( conf["Pipelines"]["default"] ) 

#pline_noBalance["RootFileFolder"] = "cut_nobalance"
#pline_noBalance["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi

#pline_onlyDeltaPhi = copy.deepcopy( conf["Pipelines"]["default"] ) 

#pline_onlyDeltaPhi["Filter"].append("cutselection")
#pline_onlyDeltaPhi["Filter2ndJetPtCutSet"] = False
#pline_onlyDeltaPhi["FilterDeltaPhiCutSet"] = True
#pline_onlyDeltaPhi["RootFileFolder"] = "cut_onlydeltaphi"
#pline_onlyDeltaPhi["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi

#conf["Pipelines"]["default_only2ndJetCut"] = pline_only2ndJet
#conf["Pipelines"]["default_noBalance"] = pline_noBalance
#conf["Pipelines"]["default_onlyDeltaPhi"] = pline_onlyDeltaPhi
#conf["Pipelines"]["default_bothcuts"] = pline_bothcuts

#conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]
#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )
JsonConfigBase.Run( conf, sys.argv[0] + ".json")
