import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf["EnableMetPhiCorrection"] = 1
cbase.Run(conf, sys.argv)
