import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
conf['Jec'] = cbase.GetBasePath() + "data/jec_data/Summer12_V7_MC"
cbase.Run(conf, sys.argv)
