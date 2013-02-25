import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf['Jec'] = cbase.GetBasePath() + "data/jec_data/Spring13_V5_DATA"
cbase.Run(conf, sys.argv)
