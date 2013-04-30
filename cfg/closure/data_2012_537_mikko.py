import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('data', '2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/8/dhaitz/Winter13ReReco_2/*.root", sys.argv)
conf["OutputPath"] = "closure_data2012_537_mikko"
conf["JsonFile"] = cbase.GetBasePath() + "data/json/Cert_190456-208686_8TeV_Combined22JanReReco_Collisions12_JSON.txt"

algorithms = ["AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf)

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3, 0.4], onlyBasicQuantities=False) ]
#variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 5, 9, 16, 22], [4, 8, 15, 21, 100], onlyBasicQuantities=False, alsoForPtBins=False) ]
variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [ 0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=True) ]

#cbase.AddCorrectionPlots(conf, base_algorithms, l3residual = True)
#cbase.AddCutConsumer(conf, algorithms)
#cbase.AddHltConsumer(conf, base_algorithms, ["HLT_Mu17_Mu8", "HLT_Mu13_Mu8", "HLT_DoubleMu7", "HLT_DoubleMu5"])

for v in variations:
    conf["Pipelines"].update(v)

#cbase.AddLumiConsumer(conf, algorithms)
#cbase.AddQuantityPlots(conf, algorithms)

cbase.Run(conf, sys.argv)
