import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf["L1Correction"] = "L1Offset"
cbase.Run(conf, sys.argv)
