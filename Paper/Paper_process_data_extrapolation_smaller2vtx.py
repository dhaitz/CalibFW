
# contains methods to create parts of the configuration which are always the same
import JsonConfigBase

# contains configs of the paths used on your machines. in this external config
# so you can run on multiple machines without changing the config file ( .. in theory )
import LocalConfigBase
import sys
import copy

# returns one pipeline with mc configs all set. Default cut values set. etc
# see the GetMcBaseConfig method for details
# also sets the Algos to process
conf = JsonConfigBase.GetDataBaseConfig()

# if you want to change the values of default values, do it now:
#
# conf["Pipelines"]["default"]["CutZMassWindow"] = 23 
# etc

# add the l1 corr to the list
conf["Algos"].append( "ak5PFJetsL1" )
conf["Algos"].append( "ak5PFJetsL1L2L3Res" )

# easy ...
conf["InputFiles"] = "/afs/naf.desy.de/user/p/piparo/lustre/Paper/ZPJ2011_data_nov4_corrected/*root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "extrapolation_data_less2vtx"

conf["Pipelines"]["default"]["Filter"].append("recovert")
conf["Pipelines"]["default"]["FilterRecoVertLow"] = 0
conf["Pipelines"]["default"]["FilterRecoVertHigh"] = 2

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

second_leading_jetpt = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.05, 0.1, 0.13, 0.15, .17, 0.2, 0.3], True, True  )
#CutBack2Back = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [.04,.14,.24,.34], True, True  )

conf["Pipelines"] = dict( conf["Pipelines"].items() + second_leading_jetpt.items() )

# creates the file process_mc_dy.py.json containing all settings and runs bin/resp_cuts.exe 
JsonConfigBase.Run( conf, sys.argv[0] + ".json")

## tada that.s it
