import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
cbase.Run(conf, sys.argv)
