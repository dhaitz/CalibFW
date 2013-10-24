import sys
import ArtusConfigBase as base

conf = base.BaseConfig('mc', '2012')
conf["InputFiles"] = base.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_background/kappa_WJets_madgraph*.root", sys.argv)
conf["OutputPath"] = "artus_bkgWJets_534"

algorithms = ["AK5PFJetsL1L2L3"]
base.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = base.ExpandConfig(algorithms, conf, expandptbins=False, alletaFolder=True, zcutsFolder=True)

base.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
base.AddCutConsumer(conf, algorithms)

base.AddQuantityPlots(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

base.Run(conf, sys.argv)
