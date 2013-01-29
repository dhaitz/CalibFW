import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
conf['GlobalProducer'].remove('pu_reweighting_producer')
cbase.Run(conf, sys.argv)
