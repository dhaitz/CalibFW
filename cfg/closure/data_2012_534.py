import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
cbase.Run(conf, sys.argv)
