import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa534_MC12_powheg/*.root", sys.argv)
cbase.ApplyPUReweighting(conf, "kappa534_MC12_powheg_190456-208357_8TeV_PromptReco")
cbase.Run(conf, sys.argv)
