
import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/berger/zjet/Run2011A-May10andPromptReco-A/*.root"
conf["OutputPath"] = "/scratch/hh/lustre/cms/user/berger/analysis/data_July22"

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,1000], conf, True )

conf["Algos"] = ["ak5PFJetsL1", "ak5PFJetsL1CHS", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3CHS", "ak5PFJetsL1L2L3Res", "ak5PFJetsL1L2L3ResCHS" ]#, "ak7PFJetsL1", 
#"ak7PFJetsL1L2L3","ak7PFJetsL1L2L3Res"]


#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
back2back_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
secjet_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )

conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items())

conf["Pipelines"] = dict( conf["Pipelines"].items() + JsonConfigBase.CreateEndcapPipelines( conf["Pipelines"] ).items())


#
#pline_only2ndJet = copy.deepcopy( conf["Pipelines"]["default"] ) 
#
#pline_only2ndJet["Filter"].append("cutselection")
#pline_only2ndJet["Filter2ndJetPtCutSet"] = True
#pline_only2ndJet["FilterDeltaPhiCutSet"] = False
#pline_only2ndJet["RootFileFolder"] = "cut_only2ndJet"
#pline_only2ndJet["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi
#
#pline_bothcuts = copy.deepcopy( conf["Pipelines"]["default"] ) 
#
#pline_bothcuts["Filter"].append("cutselection")
#pline_bothcuts["Filter2ndJetPtCutSet"] = True
#pline_bothcuts["FilterDeltaPhiCutSet"] = True
#pline_bothcuts["RootFileFolder"] = "cut_bothtopo"
#pline_bothcuts["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi
#
#pline_noBalance = copy.deepcopy( conf["Pipelines"]["default"] ) 
#
#pline_noBalance["RootFileFolder"] = "cut_nobalance"
#pline_noBalance["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi
#
#pline_onlyDeltaPhi = copy.deepcopy( conf["Pipelines"]["default"] ) 
#
#pline_onlyDeltaPhi["Filter"].append("cutselection")
#pline_onlyDeltaPhi["Filter2ndJetPtCutSet"] = False
#pline_onlyDeltaPhi["FilterDeltaPhiCutSet"] = True
#pline_onlyDeltaPhi["RootFileFolder"] = "cut_onlydeltaphi"
#pline_onlyDeltaPhi["FilterInCutIgnored"] = 48 # 2ndJet cut and DeltaPhi
#
#
#conf["Pipelines"]["default_only2ndJetCut"] = pline_only2ndJet
#conf["Pipelines"]["default_noBalance"] = pline_noBalance
#conf["Pipelines"]["default_onlyDeltaPhi"] = pline_onlyDeltaPhi
#conf["Pipelines"]["default_bothcuts"] = pline_bothcuts

conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]
#conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items() + endcap.items() )# + back2back_var.items() )



#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )
JsonConfigBase.Run( conf, sys.argv[0] + ".json")
