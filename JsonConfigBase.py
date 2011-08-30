import copy
import subprocess
import glob

def CreateFileList( wildcardExpression):
    flist = []
    
    for name in glob.glob(wildcardExpression):
        flist.append(name)
        
    return flist
    

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


def GetBaseConfig():
    d = dict()
    d["ThreadCount"] = 1
    d["Algos"] = ["ak5PFJets"]#"ak7PFJets", "ak5CaloJets", "ak7CaloJets", "kt4PFJets","kt6PFJets", "kt4CaloJets", "kt6CaloJets", "ic5PFJets", "ic5CaloJets"]
    d["Pipelines"] = { "default": {
            "Level": 1,
            "JetAlgorithm" : "AK5PFJetsL1FastL2L3",
            "RootFileFolder": "",
            "AdditionalConsumer": [],
            "CutMuonEta": 2.3,
            "CutMuonPt": 15,
            "CutZMassWindow": 20,
            "CutLeadingJetEta": 1.3,
            "CutSecondLeadingToZPt": 0.2,
            "CutBack2Back": 0.34,

            
            "Cuts": ["muon_pt",
                     "muon_eta",
                     "leadingjet_eta",
                     "secondleading_to_zpt",
                     "back_to_back",
                     "zmass_window"],
            "Filter":["valid_z", "valid_jet"],
            "Consumer": {}
                      }
            }
    
    return d


def ApplyReweightingSummer11May10ReReco(conf):

    conf["GlobalXSection"] = 1614.0
    conf["EnablePuReweighting"] = 1
    conf["RecovertWeight"] = [0.2634339699, 0.4068300319, 1.0258412624,
        1.5039872842, 2.1501353803, 1.9674930073, 1.7357207863, 1.5885466557,
        1.2814939016, 0.8379304030, 0.5751357475, 0.3933389880, 0.2618616395,
        0.1928669420, 0.1178827060, 0.0989967695, 0.0707225141, 0.0494813344,
        0.0630199599, 0.0275894575, 0.0189547094, 0.0708500595, 0.0581618600,
        0.0115549447, 0.0094252128]
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
    
    d["JsonFile"] = "data/json/Cert_160404-163869_7TeV_May10ReReco_Collisions11_JSON.txt"
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
        
        cutPipe["Consumer"]["bin_mpf_response"] = { "Name" : "bin_response",
                                                        "ProductName" : "mpfresp_AK5PFJetsL1FastL2L3",
                                                        "ResponseType" : "mpf", 
                                                         "JetNumber" : 0}
        
        cutPipe["Consumer"]["bin_balance_response"] = { "Name" : "bin_response",
                                                        "ResponseType" : "bal",
                                                        "ProductName" : "balresp_AK5PFJetsL1FastL2L3",
                                                        "JetNumber" : 1 }
        cutPipe["Consumer"]["bin_balance_response_2ndJet"] = { "Name" : "bin_response",
                                                              "ResponseType" : "bal",
                                                        "ProductName" : "bal_jet2_z_AK5PFJetsL1FastL2L3",
                                                        "JetNumber" : 2 }


        # only add the nocut pipeline for the default ( no binning )
        print name
        if name == "default":
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

        if not name == "defaultnocuts":
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
    
def ExpandDefaultMcConfig( ptBins, conf_template, useFolders, FolderPrefix = ""):
    conf = conf_template

    #conf["Pipelines"]["default"]["CustomBins"] = ptBins
    conf["Pipelines"] = ExpandCutNoCut( conf["Pipelines"] )    

    secLevelPline = { FolderPrefix + "sec_default": copy.deepcopy( conf["Pipelines"]["default"] )}
    
    secpline = secLevelPline[FolderPrefix + "sec_default"]    
    
    srcFolder = []
    for i in range( len(ptBins) - 1):
        srcFolder += ["Pt" + str(ptBins[i]) + "to" + str(ptBins[i+1]) + "_incut"]
    
    # code this in a more generic way
    secpline["Consumer"] = {}
    secpline["Consumer"]["bal_response"] = { "Name" : "response_balance",
                                         "SourceFolder" : srcFolder,
                                         "SourceResponse" : "balresp_AK5PFJetsL1FastL2L3",
                                         # this product will be in the upmost folder
                                         "ProductName"    : "balresp_AK5PFJetsL1FastL2L3",
                                         "SourceBinning"  : "z_pt_AK5PFJetsL1FastL2L3"}
    
    secpline["Consumer"]["mpf_response"] = { "Name" : "response_balance",
                                         "SourceFolder" : srcFolder,
                                         "SourceResponse" : "mpfresp_AK5PFJetsL1FastL2L3",
                                         # this product will be in the upmost folder
                                         "ProductName"    : "mpfresp_AK5PFJetsL1FastL2L3",
                                         "SourceBinning"  : "z_pt_AK5PFJetsL1FastL2L3"}
    secpline["Level"] = 2
    #secLevelPline[FolderPrefix + "sec_default"]["CustomBins"] = ptBins
    secpline["SecondLevelFolderTemplate"] = FolderPrefix + "XXPT_BINXX_incut"
    secpline["RootFileFolder"] = FolderPrefix
    
    

    conf["Pipelines"] = ExpandPtBins(  conf["Pipelines"], ptBins, True )

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
    subprocess.call(["./resp_cuts",filename])
    try:
        import pynotify
        if pynotify.init("CalibFW resp_cuts"):
            n = pynotify.Notification("CalibFW resp_cuts", "run with config " + filename + " done")
            n.show()
    except:
        pass

