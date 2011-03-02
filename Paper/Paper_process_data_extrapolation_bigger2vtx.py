import JsonConfigBase
import LocalConfigBase
import sys
import copy
conf = JsonConfigBase.GetDataBaseConfig()

conf["Algos"].append( "ak5PFJetsL1" )
conf["Algos"].append( "ak5PFJetsL1L2L3Res" )

conf["InputFiles"] = "/afs/naf.desy.de/user/p/piparo/lustre/Paper/ZPJ2011_data_nov4_corrected/*root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "extrapolation_data_more2vtx"

conf["Pipelines"]["default"]["Filter"].append("recovert")
conf["Pipelines"]["default"]["FilterRecoVertLow"] = 3
conf["Pipelines"]["default"]["FilterRecoVertHigh"] = 100

conf = JsonConfigBase.ExpandDefaultDataConfig( [0,15,30,60,100,500], conf, True )

second_leading_jetpt = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.05, 0.1, 0.13, 0.15, .17, 0.2, 0.3], True, True  )

conf["Pipelines"] = dict( conf["Pipelines"].items() + second_leading_jetpt.items() )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

## tada that.s it
