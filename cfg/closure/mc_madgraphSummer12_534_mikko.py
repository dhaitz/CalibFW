import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('mc', '2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_madgraph/*.root", sys.argv)
conf['Jec'] = cbase.GetBasePath() + "data/jec_data/Fall12_V5_MC"
conf["OutputPath"] = "closure_madgraphSummer12_534"

algorithms = ["AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf, expandptbins=True, alletaFolder=True, zcutsFolder=True)
cbase.ApplyPUReweighting(conf, "kappa534_MC12_madgraph_190456-208357_8TeV_PromptReco")
# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3, 0.4], onlyBasicQuantities=False) ]
#variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=True) ]

#cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
#cbase.AddCutConsumer(conf, algorithms)

for v in variations:
    conf["Pipelines"].update(v)

#cbase.AddQuantityPlots(conf, algorithms)

cbase.Run(conf, sys.argv)
