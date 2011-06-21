import copy
import subprocess

def getDefaultCorrectionL2( data_path ):
  globalTag = "GR_R_311_V2_"
  
  g_l2_correction_data=["ak5PFJets:" + data_path + "jec_data/" + globalTag + "AK5PF_L2Relative.txt",
      "ak7PFJets:" + data_path + "jec_data/" + globalTag + "AK7PF_L2Relative.txt",
      "kt4PFJets:" + data_path + "jec_data/" + globalTag + "KT4PF_L2Relative.txt",
      "kt6PFJets:" + data_path + "jec_data/" + globalTag + "KT6PF_L2Relative.txt",
      "ak5CaloJetst:" + data_path + "jec_data/" + globalTag + "AK5Calo_L2Relative.txt",
      "ak7CaloJets:" + data_path + "jec_data/" + globalTag + "AK7Calo_L2Relative.txt",
      "kt4CaloJets:" + data_path + "jec_data/" + globalTag + "KT4Calo_L2Relative.txt",
      "kt6CaloJets:" + data_path + "jec_data/" + globalTag + "KT6Calo_L2Relative.txt",
    "iterativeCone5PFJets:" + data_path + "jec_data/" + globalTag + "IC5PF_L2Relative.txt",
    "iterativeCone5CaloJets:" + data_path + "jec_data/" + globalTag + "IC5PF_L2Relative.txt"]
    
  return g_l2_correction_data

def GetGitInformation():
    gitlog = subprocess.Popen("git --no-pager log -n 1" , stdout=subprocess.PIPE, shell=True).stdout.read()
    gitremote = subprocess.Popen("git remote -v" , stdout=subprocess.PIPE, shell=True).stdout.read()

    return (gitlog + "\n" + gitremote)

def GetBaseConfig():
    d = dict()
    d["GitInformation"] = GetGitInformation()
    d["ThreadCount"] = 1
    d["Algos"] = ["ak5PFJets"]#"ak7PFJets", "ak5CaloJets", "ak7CaloJets", "kt4PFJets","kt6PFJets", "kt4CaloJets", "kt6CaloJets", "ic5PFJets", "ic5CaloJets"]
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

def GetMikkoCuts( conf ):
    conf["Pipelines"]["default"]["Cuts"].append("jet_pt")
    conf["Pipelines"]["default"]["CutJetPt"] = 10.0
    conf["Pipelines"]["default"]["CutSecondLeadingToZPt"] = 0.3
    conf["Pipelines"]["default"]["CutSecondLeadingToZPtJet2Threshold"] = 5.0
    conf["Pipelines"]["default"]["CutBack2Back"] = 1.047

    return conf


def GetMcBaseConfig():
    d = GetBaseConfig()
    
    d["UseWeighting"] = 1
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0
    
    d["InputType"] = "mc"

    return d

def GetDefaultDataPipeline():
    pline = GetDataBaseConfig()["Pipelines"]["default"]

    pline["FilterInCutIgnored"] = 0
    pline["Filter"].append ("incut")
    
    return pline

def GetDataBaseConfig():
    d = GetBaseConfig()
    
    d["JsonFile"] = "data/json/Cert_160404-166861_7TeV_PromptReco_Collisions11_JSON.txt"
    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0
    d["InputType"] = "data"
    d["Pipelines"]["default"]["CutHLT"] = "DoubleMu"
    
    for key, val in d["Pipelines"].items():
        val["Cuts"].append( "hlt" ) 
        val["Cuts"].append( "json" )

    return d

# does not work right now
def ExpandRange( pipelineDict, varName, vals, setRootFolder, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        
        if elem["Level"] == 1:            
            for v in vals:
                #print( elem )
                newPipe = copy.deepcopy(elem)
                #print( newPipe )
                newPipe[ varName ] = v
                
                varadd = "_var_" + varName + "_" + str(v).replace(".", "_")
                
                newName = name + varadd            
                newRootFileFolder =  newPipe["RootFileFolder"] + varadd
                
                newDict[newName] = newPipe
                if ( setRootFolder ):
                    newDict[newName]["RootFileFolder"] = newRootFileFolder


    if includeSource:
        return dict( pipelineDict.items() +  newDict.items() )
    else:
        return newDict

def ExpandCutNoCut( pipelineDict):
    newDict = dict()

    for name, elem in pipelineDict.items():
        
        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        cutPipe["FilterInCutIgnored"] = 0
        
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
    
def AddQualityCuts( conf ):

    # cuts to obey
    # json 1
    # hlt 512
    # muon eta 4
    # zmass 64
    # jet eta 8
    
    # cuts to ignore 
    # 2nd Jet Pt          16
    # muon pt cut         2
    # back to back cut    32 
    
    # bitmask :  0011 0010 = 50

   pline_qualitycuts = copy.deepcopy( conf["Pipelines"]["default"] )
   pline_qualitycuts["FilterInCutIgnored"] = 50        
   conf["Pipelines"]["NoBinning_qualitycuts"] = pline_qualitycuts
   
   return

    
def ExpandDefaultMcConfig( ptBins, conf_template, useFolders, FolderPrefix = ""):
    conf = conf_template

    conf["Pipelines"]["default"]["CustomBins"] = ptBins
    conf["Pipelines"] = ExpandCutNoCut( conf["Pipelines"] )

    secLevelPline = { FolderPrefix + "sec_default": copy.deepcopy( conf["Pipelines"]["default"] )}
    secLevelPline[FolderPrefix + "sec_default"]["Level"] = 2
    secLevelPline[FolderPrefix + "sec_default"]["CustomBins"] = ptBins
    secLevelPline[FolderPrefix + "sec_default"]["SecondLevelFolderTemplate"] = FolderPrefix + "XXPT_BINXX_incut"
    secLevelPline[FolderPrefix + "sec_default"]["RootFileFolder"] = FolderPrefix

    conf["Pipelines"] = ExpandPtBins(  conf["Pipelines"], ptBins, True )

    AddQualityCuts( conf )

    #merge all
    if useFolders:
        for p, pval in conf["Pipelines"].items():

            ptVal = "NoBinning"

            if "ptbin" in pval["Filter"]:
                ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])

            if "incut" in pval["Filter"]:
                ptVal = ptVal + "_incut"
            else:
                ptVal = ptVal + "_allevents"

            pval["RootFileFolder"] = FolderPrefix + ptVal

    for (key, val) in conf["Pipelines"].items():
        secLevelPline[ FolderPrefix + key ] = val

    conf["Pipelines"] = secLevelPline

    return conf

    
    
def ExpandDefaultDataConfig( ptBins, conf_template, useFolders, FolderPrefix = ""):
    conf = ExpandDefaultMcConfig( ptBins, conf_template, useFolders, FolderPrefix)

    conf["Pipelines"][FolderPrefix + "default"]["AdditionalConsumer"].append( "event_storer" )

    return conf

def StoreSettings( settings, filename):
    f = open(filename, "w")
    
    jsonOut = str(settings)
    # make it json conform
    jsonOut = jsonOut.replace( "\'", "\"")
    
    try:
        import json
        # dont display config on console, it is annyoing
        #print json.dumps( settings, sort_keys=True, indent=4 )
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
    try:
        import pynotify
        if pynotify.init("CalibFW resp_cuts"):
            n = pynotify.Notification("CalibFW resp_cuts", "run with config " + filename + " done")
            n.show()
    except:
        pass

