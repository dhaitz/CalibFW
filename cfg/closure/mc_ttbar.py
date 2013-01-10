import sys
import ClosureConfigBase as cbase

conf = cbase.BaseConfig('mc', '2012')
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_background/kappa_TTJets*.root", sys.argv)
conf["OutputPath"] = "closure_bkgQCD_534"

algorithms = ["AK5PFJetsL1L2L3"]
cbase.addCHS(algorithms)
base_algorithms = ["AK5PFJets", "AK5PFJetsCHS"]

conf = cbase.ExpandConfig(algorithms, conf, expandptbins=False, alletaFolder=True, zcutsFolder=True)

cbase.AddCorrectionPlots(conf, base_algorithms, l3residual=False)
cbase.AddCutConsumer(conf, algorithms)

cbase.AddQuantityPlots(conf, ["AK5PFJetsL1L2L3", "AK5PFJetsCHSL1L2L3"], forIncut=True, forAllevents=True, forIncutVariations=True, forAlleventsVariations=False)

cbase.Run(conf, sys.argv)
