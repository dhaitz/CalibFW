import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('data', '2011')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa44_Data11/*.root", sys.argv)
conf["OutputPath"] = "closure_data_2011"

algorithms = ["AK5PFJets", "AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf, expandptbins=False)

# create various variations ...
variations = []
#variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3]) ]
#variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 3, 6, 12], [2, 5, 11, 100]) ]
#variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.522, 1.305, 1.930, 2.411, 2.853, 3.139], [0.522, 1.305, 1.930, 2.411, 2.853, 3.139, 5.0]) ]

#cbase.AddCorrectionPlots(conf, base_algorithms, l3residual = True)
#cbase.AddCutConsumer(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3", "AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"])
#cbase.AddHltConsumer(conf, base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5"])
#cbase.AddLumiConsumer(conf, base_algorithms)

for v in variations:
    conf["Pipelines"].update(v)

cbase.Add2DHistograms(conf, ["AK5PFJets", "AK5PFJetsCHS", "AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], incut=True, allevents=True, all_variations=True)
cbase.Add2DProfiles(conf, ["AK5PFJets", "AK5PFJetsCHS", "AK5PFJetsL1L2L3Res", "AK5PFJetsCHSL1L2L3Res"], incut=True, allevents=True, all_variations=True)

cbase.Run(conf, sys.argv)
