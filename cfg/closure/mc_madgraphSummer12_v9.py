import sys
import ClosureConfigBase as cbase

conf = cbase.GetMcBaseConfig(run='2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa52_MC12_madgraphV9/*.root", sys.argv)
conf["OutputPath"] = "closure_mc_madgraphSummer12"

algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandDefaultMcConfig(algorithms, conf)
cbase.ApplyPUReweighting(conf, "kappa52_MC12_madgraphV9_190456-195947_8TeV_PromptReco_v2")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3]) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100]) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964], [0.783, 1.305, 1.93, 2.5, 2.964, 5.191]) ]

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"])

for v in variations:
    conf["Pipelines"].update(v)

cbase.Run(conf, sys.argv)
