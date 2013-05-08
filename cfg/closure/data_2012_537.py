import sys
import ClosureConfigBase as cbase
import data_template

conf = data_template.get_template()
conf["InputFiles"] = cbase.CreateFileList("/storage/8/dhaitz/Winter13ReReco_2/*.root", sys.argv)
conf["OutputPath"] = "closure_data2012_537"
conf["JsonFile"] = cbase.GetBasePath() + "Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
cbase.Run(conf, sys.argv)
