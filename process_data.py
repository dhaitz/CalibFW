
import JsonConfigBase


conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/local/scratch/hauth/data/ZPJ2010/skim_data/data*_job_Mu_43*.root" 
conf["OutputPath"] = "proto"

pipesPtCutNocut = JsonConfigBase.ExpandCutNoCut( conf["Pipelines"])
pipesPtBin = JsonConfigBase.ExpandPtBins( pipesPtCutNocut, [0,10,20,30,100])

conf["Pipelines"] = pipesPtBin
conf["Pipelines"] = dict( conf["Pipelines"].items() +  pipesPtCutNocut.items() )
f = open("conf.json", "w")

conf["Pipelines"] = JsonConfigBase.Expand( conf["Pipelines"], 10)


print ( "Configured " + str( len( conf["Pipelines"] )) + " Pipelines" )

jsonOut = str(conf)
jsonOut = jsonOut.replace( "\'", "\"")

f.write ( jsonOut )
f.close()
