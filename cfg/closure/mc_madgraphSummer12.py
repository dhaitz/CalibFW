import sys
import ClosureConfigBase as cbase

conf = cbase.GetMcBaseConfig(run='2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa52_MC12/*.root", sys.argv)
conf["OutputPath"] = "closure_mc_madgraphSummer12"

algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3", "AK7PFJets", "AK7PFJetsL1", "AK7PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS", "AK7PFJets", "AK7PFJetsCHS"]

conf["GlobalAlgorithms"] += ["AK7PF", "AK7PFchs"]

conf = cbase.ExpandDefaultMcConfig(algorithms, conf)
cbase.ApplyPUReweighting(conf, "kappa52_MC12_190456-196531_8TeV_PromptReco")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3, 04], onlyBasicQuantities=False) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False) ]

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3", "AK7PFJets", "AK7PFJetsL1", "AK7PFJetsL1L2L3", "AK7PFJetsCHSL1L2L3"])

for v in variations:
    conf["Pipelines"].update(v)

cbase.Run(conf, sys.argv)
