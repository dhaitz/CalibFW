import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf["InputFiles"] = cbase.CreateFileList("/storage/8/dhaitz/Winter13ReReco_2/*.root", sys.argv)
conf["JsonFile"] = cbase.GetBasePath() + "data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
conf['Jec'] = cbase.GetBasePath() + "data/jec_data/Summer13_V4_DATA"
conf["OutputPath"] = "closure_data2012_537_Summer13_V4"
cbase.Run(conf, sys.argv)
