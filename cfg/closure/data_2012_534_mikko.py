import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('data', '2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa534_Data12/*.root", sys.argv)
conf["OutputPath"] = "closure_data2012_534_mikko"

algorithms = ["AK5PFJetsL1L2L3", "AK5PFJetsL1L2L3Res"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf)

variations = [cbase.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3, 0.4], onlyBasicQuantities=False)]
variations += [cbase.ExpandRange2(variations[0], "JetEta", [0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139], [0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191], onlyBasicQuantities=False, alsoForPtBins=True)]

for v in variations:
    conf["Pipelines"].update(v)

cbase.Run(conf, sys.argv)
