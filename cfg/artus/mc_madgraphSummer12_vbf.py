import sys
import ArtusConfigBase as base

conf = base.BaseConfig('mc', '2012', 'vbf')
conf["InputFiles"] = base.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_madgraph/*.root", sys.argv)
conf["OutputPath"] = "artus_madgraphSummer12_vbf"

algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3"]
base.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = base.ExpandConfig(algorithms, conf, expandptbins=True, alletaFolder=True, zcutsFolder=True)

# create various variations ...
variations = []
variations += [ base.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.2, 0.3, 0.35, 0.4], onlyBasicQuantities=False) ]
variations += [ base.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ base.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False, onlyOnIncut=False) ]
variations += [ base.ExpandRange2(conf["Pipelines"], "Flavour", [123, 4, 5, 21], onlyBasicQuantities=True, alsoForPtBins=False, onlyOnIncut=True) ]

base.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
base.AddCutConsumer(conf, algorithms)

for v in variations:
    conf["Pipelines"].update(v)

base.AddQuantityPlots(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

base.Add2DHistograms(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)
base.Add2DProfiles(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

base.Run(conf, sys.argv)
