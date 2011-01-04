import copy
import subprocess

def GetBaseConfig():
    d = dict()
    
    d["Algos"] = ["ak5PFJets","ak7PFJets", "ak5CaloJets", "ak7CaloJets", "kt4PFJets","kt6PFJets", "kt4CaloJets", "kt6CaloJets", "ic5PFJets", "ic5CaloJets"]
    d["Pipelines"] = { "default": {
            "Level": 1,
            "RootFileFolder": "",
            "AdditionalConsumer": [],
            "CutMuonEta": 2.3,
            "CutMuonPt": 15,
            "CutZMassWindow": 20,
            "CutLeadingJetEta": 1.3,
            "CutSecondLeadingToZPt": 0.2,
            "CutSecondLeadingToZPtJet2Threshold" : 5.0,
            "CutBack2Back": 0.34,
            "Cuts": ["muon_pt",
                     "muon_eta",
                     "leadingjet_eta",
                     "secondleading_to_zpt",
                     "back_to_back",
                     "zmass_window"],
            "Filter":[]
                      }
            }
            
    
    return d

def GetMcBaseConfig():
    d = GetBaseConfig()
    
    d["UseWeighting"] = 1
    d["UseEventWeight"] = 0
    d["InputType"] = "mc"

    return d

def GetDataBaseConfig():
    d = GetBaseConfig()
    
    d["JsonFile"] = "data/json/Cert_132440-149442_7TeV_StreamExpress_Collisions10_JSON_v3.txt"
    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["InputType"] = "data"
    
    for key, val in d["Pipelines"].items():
        val["Cuts"].append( "hlt" ) 
        val["Cuts"].append( "json" )

    return d

# does not work right now
def ExpandRange( pipelineDict, varName, vals, setRootFolder, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        for v in vals:
            print( elem )
            newPipe = copy.deepcopy(elem)
            print( newPipe )
            newPipe[ varName ] = v
            
            newName = name + "var_" + varName + "_" + str(v).replace(".", "_")
            
            newDict[newName] = newPipe
            if ( setRootFolder ):
                newDict[newName]["RootFileFolder"] = newName


    if includeSource:
        return dict( pipelineDict.items() +  newDict.items() )
    else:
        return newDict

def ExpandCutNoCut( pipelineDict):
    newDict = dict()

    for name, elem in pipelineDict.items():
        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        
        cutPipe["Filter"].append ("incut")

        newDict[name + "nocuts" ] = nocutPipe
        newDict[name] = cutPipe

    return newDict

def Expand( pipelineDict, expandCount, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        for i in range( expandCount):
            newPipe = copy.deepcopy(elem)
            newDict[name + str(i) ] = newPipe
    
    if includeSource:
        return dict( pipelineDict.items() +  newDict.items() )
    else:
        return newDict

def ExpandPtBins( pipelineDict, ptbins, includeSource):
    newDict = dict()
    
    
    
    for name, elem in pipelineDict.items():
        i = 0
        for upper in ptbins[1:]:
            ptbinsname =  str(ptbins[i]) + "to" + str(upper)

            newPipe = copy.deepcopy(elem)
            
            newPipe["Filter"].append( "ptbin")
            
            newPipe["FilterPtBinLow"] = ptbins[i]
            newPipe["FilterPtBinHigh"] = upper

            newDict[name + ptbinsname ] = newPipe
            i = i + 1

    if includeSource:
        return dict( pipelineDict.items() +  newDict.items() )
    else:
        return newDict
    
    
    
def ExpandDefaultDataConfig( ptBins, conf_template, useFolders):
    conf = conf_template
    
    conf["Pipelines"]["default"]["CustomBins"] = ptBins
    conf["Pipelines"] = ExpandCutNoCut( conf["Pipelines"] )
    
    secLevelPline = { "sec_default": copy.deepcopy( conf["Pipelines"]["default"] )}
    secLevelPline["sec_default"]["Level"] = 2
    secLevelPline["sec_default"]["CustomBins"] = ptBins

    conf["Pipelines"] = ExpandPtBins(  conf["Pipelines"], ptBins, True )
    
    #merge all
    conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics", "event_storer"]

    if useFolders:
        for p, pval in conf["Pipelines"].items():
            
            ptVal = "NoBinning"
            
            if "ptbin" in pval["Filter"]:
                ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])  
                
            if "incut" in pval["Filter"]:
                ptVal = ptVal + "_incut"
            else:
                ptVal = ptVal + "_allevents"           
            
            pval["RootFileFolder"] = ptVal

        
    conf["Pipelines"] = dict( conf["Pipelines"].items() +  secLevelPline.items() )

    return conf

def StoreSettings( settings, filename):
    f = open(filename, "w")
    
    jsonOut = str(settings)
    # make it json conform
    jsonOut = jsonOut.replace( "\'", "\"")
    
    try:
        import json
        print json.dumps( settings, sort_keys=True, indent=4 )
        json.dump( settings, f, sort_keys=True, indent=4 )
            
    except BaseException:
    
        f.write ( jsonOut )
    
        print "No json Module found. Using fallback method ..."
        
    f.close()
    
    print ( "Configured " + str( len( settings["Pipelines"] )) + " Pipelines" )
    
    
def Run( settings, filename):    
    StoreSettings( settings, filename)
    print "Running config from file " + filename
    subprocess.call(["bin/resp_cuts.exe",filename])

    
