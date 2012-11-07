import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('mc', '2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa53_MC12_madgraph/*.root", sys.argv)
conf['Jec'] = cbase.GetBasePath() + "data/jec_data/Fall12_V4_MC"
conf["OutputPath"] = "closure_mc_madgraphSummer12_53X_Fall12JEC_V4"
conf["UseMETPhiCorrection"] = 0

algorithms = ["AK7PFJets", "AK7PFJetsL1", "AK7PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK7PFJets", "AK7PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf, expandptbins=True, alletaFolder=True, zcutsFolder=True)
cbase.ApplyPUReweighting(conf, "kappa53_MC12_madgraph_190456-203853_8TeV_CombinedReco_v2")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.2, 0.3, 0.35, 0.4], onlyBasicQuantities=False) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False, onlyOnIncut=False) ]

cbase.AddCutConsumer(conf, algorithms)

for v in variations:
    conf["Pipelines"].update(v)

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False, forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

cbase.AddQuantityPlots(conf, ["AK7PFJets", "AK7PFJetsCHS", "AK7PFJetsCHSL1", "AK7PFJetsL1L2L3", "AK7PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False, isMC=True)

cbase.Add2DHistograms(conf, ["AK7PFJetsL1L2L3", "AK7PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)
cbase.Add2DProfiles(conf, ["AK7PFJetsL1L2L3", "AK7PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False, isMC=True)

cbase.Run(conf, sys.argv)
