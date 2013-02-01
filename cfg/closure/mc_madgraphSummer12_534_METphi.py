import sys
import ClosureConfigBase as cbase
import mc_template

conf = mc_template.get_template()
conf["EnableMetPhiCorrection"] = 1
cbase.Run(conf, sys.argv)
