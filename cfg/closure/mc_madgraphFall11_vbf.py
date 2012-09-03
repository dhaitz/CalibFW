import sys
import ClosureConfigBase as cbase

conf = cbase.GetMcBaseConfig('vbf')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa44_MC11_madgraph/*.root", sys.argv)
conf["OutputPath"] = "closure_mc_madgraphFall11_vbf"

algorithms = ["AK5PFJets", "AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf)
cbase.ApplyPUReweighting(conf, "kappa44_MC11_madgraph_160404-180252_7TeV_ReRecoNov08")

# create various variations ...
variations = []
variations += [ cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3]) ]
variations += [ cbase.ExpandRange2(variations[0], "Npv", [0, 3, 6, 12], [2, 5, 11, 100]) ]
#variations += [ cbase.ExpandRange2(variations[0], "JetEta", [0, 0.522, 1.305, 1.930, 2.411, 2.853, 3.139], [0.522, 1.305, 1.930, 2.411, 2.853, 3.139, 5.0]) ]

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"])

for v in variations:
    conf["Pipelines"].update(v)

cbase.Run(conf, sys.argv)
