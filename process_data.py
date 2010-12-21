
import JsonConfigBase
import copy


conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/local/scratch/hauth/data/ZPJ2010/skim_data/data*_job_Mu_*.root" 
conf["OutputPath"] = "proto"

conf["Pipelines"] = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.1, 0.2, 0.3], True )
conf["Pipelines"] = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.2, 0.3], True )

#pipesPtCutNocut = JsonConfigBase.ExpandCutNoCut( conf["Pipelines"])
pipesPtBin = JsonConfigBase.ExpandPtBins(  conf["Pipelines"], [0,10,20,30,100])

conf["Pipelines"] = pipesPtBin
conf["Pipelines"] = dict( conf["Pipelines"].items() +  pipesPtCutNocut.items() )
f = open("conf.json", "w")

#conf["Pipelines"] = JsonConfigBase.Expand( conf["Pipelines"], 10)

print ( "Configured " + str( len( conf["Pipelines"] )) + " Pipelines" )

jsonOut = str(conf)
jsonOut = jsonOut.replace( "\'", "\"")

f.write ( jsonOut )
f.close()
