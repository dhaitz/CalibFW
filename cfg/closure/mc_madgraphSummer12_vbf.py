import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('mc', '2012', 'vbf')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa52_MC12/*.root", sys.argv)
conf["OutputPath"] = "closure_mc_madgraphSummer12_vbf"

algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf)
cbase.ApplyPUReweighting(conf, "kappa52_MC12_190456-201678_8TeV_PromptReco")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3, 0.4], onlyBasicQuantities=False) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False) ]

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, algorithms)

for v in variations:
    conf["Pipelines"].update(v)

cbase.AddQuantityPlots(conf, algorithms)

cbase.Run(conf, sys.argv)
