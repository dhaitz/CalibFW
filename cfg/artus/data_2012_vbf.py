import sys
import ArtusConfigBase as base

conf = base.BaseConfig('data', '2012', 'vbf')
conf["InputFiles"] = base.CreateFileList("/storage/6/berger/zpj/kappa534_Data12/*.root", sys.argv)
conf["OutputPath"] = "artus_data2012_vbf"

algorithms = ["AK5PFJets", "AK5PFJetsL1", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res"]
base.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = base.ExpandConfig(algorithms, conf, expandptbins=True, alletaFolder=True, zcutsFolder=True)

# create various variations ...
variations = []
variations += [base.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.2, 0.3, 0.35, 0.4], onlyBasicQuantities=False)]
variations += [base.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False)]
variations += [base.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=False, onlyOnIncut=False)]

base.AddCorrectionPlots(conf, base_algorithms, l3residual=True)
base.AddCutConsumer(conf, algorithms)
base.AddHltConsumer(conf, base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5"])

for v in variations:
    conf["Pipelines"].update(v)

base.AddLumiConsumer(conf, ["AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=True)
base.AddQuantityPlots(conf, ["AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

base.Add2DHistograms(conf, ["AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=True)
base.Add2DProfiles(conf, ["AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=True)

base.Run(conf, sys.argv)
