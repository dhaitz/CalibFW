import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf["HcalCorrection"] = 3.0e-5

cbase.Run(conf, sys.argv)
