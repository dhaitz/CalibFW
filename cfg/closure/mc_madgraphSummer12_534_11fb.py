import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
cbase.ApplyPUReweighting(conf, 'kappa534_MC12_madgraph_')
cbase.Run(conf, sys.argv)
