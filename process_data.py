
import JsonConfigBase
import copy


conf = JsonConfigBase.GetDataBaseConfig()

conf["InputFiles"] = "/local/scratch/hauth/data/ZPJ2010/skim_data/data*_job_Mu_4*.root" 
conf["OutputPath"] = "proto"

defaultPline = copy.deepcopy( conf["Pipelines"] )
secLevelPline = copy.deepcopy( conf["Pipelines"] )

secLevelPline["default"]["Level"] = 2
secLevelPline["default"]["JetResponseBins"] = [0,10,20,30,100]

#conf["Pipelines"] = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.1, 0.2, 0.3], True )
#conf["Pipelines"] = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.2, 0.3], True )

conf["Pipelines"] = JsonConfigBase.ExpandCutNoCut( conf["Pipelines"])
conf["Pipelines"] = JsonConfigBase.ExpandPtBins(  conf["Pipelines"], [0,10,20,30,100])

#merge all
conf["Pipelines"] = dict( conf["Pipelines"].items() +  defaultPline.items() )
conf["Pipelines"] = dict( conf["Pipelines"].items() +  secLevelPline.items() )



f = open("conf.json", "w")

#conf["Pipelines"] = JsonConfigBase.Expand( conf["Pipelines"], 10)

print ( "Configured " + str( len( conf["Pipelines"] )) + " Pipelines" )

jsonOut = str(conf)
# make it json conform
jsonOut = jsonOut.replace( "\'", "\"")

f.write ( jsonOut )
f.close()
