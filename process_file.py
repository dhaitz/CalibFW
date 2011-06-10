
import JsonConfigBase
import LocalConfigBase
import sys
import copy

def PrintUsageAndExit():
        print "Usage: process_files.py <mc|data> <filenames with wildcard> <algoname> [jsonfile]"
        exit(0)

if (len(sys.argv) < 3):
	PrintUsageAndExit()

inptype = sys.argv[1]
files = sys.argv[2]
algoname = sys.argv[3].split(":")

if inptype == "data":
	if len(sys.argv) < 5 :
		print "Json file for data needed."
		exit(0)
	else:
		jsonfile = sys.argv[4]

if not (inptype == "data" or inptype == "mc" ):
	PrintUsageAndExit()

if inptype == "data": 
	conf = JsonConfigBase.GetMcBaseConfig()
if inptype == "mc":
	conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"] = algoname

conf["InputFiles"] = files
conf["OutputPath"] = "files_out"

conf["UseWeighting"] = 0

if inptype == "mc":
	conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,300], conf, True )
if inptype == "data":
	conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,300], conf, True )

# additional consumer
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
