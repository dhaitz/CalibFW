import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
cbase.ApplyPUReweighting(conf, 'kappa534_MC12_madgraph_190456-208686_8TeV_Combined22JanReReco_D')
cbase.Run(conf, sys.argv)
