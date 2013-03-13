import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf['JsonFile'] = cbase.GetBasePath() + "data/json/11fb.txt"
cbase.Run(conf, sys.argv)
