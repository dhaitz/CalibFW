import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
conf["L1Correction"] = "L1Offset"
cbase.Run(conf, sys.argv)
