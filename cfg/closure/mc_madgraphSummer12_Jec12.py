import sys
import ClosureConfigBase as cbase

conf = cbase.GetMcBaseConfig(run='2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa52_MC12_madgraphV9/*.root", sys.argv)
conf["OutputPath"] = "closure_mc_madgraphSummer12"
conf["JecBase"] = cbase.GetBasePath() + "data/jec_data/Jec12_V7"

algorithms = ["AK5PFJets", "AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandDefaultMcConfig(algorithms, conf)
cbase.ApplyPUReweighting(conf, "kappa52_MC12_madgraphV9_190456-194076_8TeV_PromptReco")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3]) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 3, 6, 12], [2, 5, 11, 100]) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 1.305, 2.411], [1.305, 2.411, 5.0]) ]

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"])

for v in variations:
    conf["Pipelines"].update(v)

cbase.Run(conf, sys.argv)
