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
    d["Pipelines"] = { "default": {
            "Level": 1,
            "JetAlgorithm" : "to_set",
            "RootFileFolder": "",
            "AdditionalConsumer": [],
            "CutMuonEta": 2.3,
            "CutMuonPt": 15,
            "CutZMassWindow": 20,
            "CutLeadingJetEta": 1.3,
            "CutSecondLeadingToZPt": 0.2,
            "CutBack2Back": 0.34,

            
            "Cuts": ["muon_pt"],
 #                    "muon_eta",
#                     "leadingjet_eta",
                     #"secondleading_to_zpt",
                     #"back_to_back",
#                     "zmass_window"],
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
    
    d["JecBase"] = "data/jec_data/MC_42_V13_"

    return d

def GetDefaultDataPipeline():
    pline = GetDataBaseConfig()["Pipelines"]["default"]

    pline["FilterInCutIgnored"] = 0    
    pline["Filter"].append ("incut")
    
    return pline

def GetDataBaseConfig():
    d = GetBaseConfig()
    
    d["JsonFile"] = "data/json/Cert_160404-178677_7TeV_PromptReco_Collisions11_JSON.txt"
    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0
    d["InputType"] = "data"
    d["Pipelines"]["default"]["CutHLT"] = "DoubleMu"
    d["Pipelines"]["default"]["Filter"].append ("json")
    
    d["JecBase"] = "data/jec_data/GR_R_42_V19_"

    #for key, val in d["Pipelines"].items():
      #  "Filter":["valid_z", "valid_jet"]
       # val["Cuts"].append( "hlt" ) 
       # val["Cuts"].append( "json" )

    return d

# does not work right now
def ExpandRange( pipelineDict, varName, vals, setRootFolder, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        
        if elem["Level"] == 1:            
            for v in vals:
                newPipe = copy.deepcopy(elem)
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

def AddConsumer( pline, name, config):
    pline["Consumer"][name] = config

def AddConsumerEasy( pline, consumer):
    pline["Consumer"][ consumer["ProductName"] ] = consumer  

def ExpandCutNoCut( pipelineDict):
    newDict = dict()

    for name, elem in pipelineDict.items():
        
        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        
        algoName = cutPipe["JetAlgorithm"]
        
        cutPipe["FilterInCutIgnored"] = 0
        cutPipe["Filter"].append ("incut")        
        
        cutPipe["Consumer"]["bin_mpf_response"] = { "Name" : "bin_response",
                                                        "ProductName" : "mpfresp_" +  algoName,
                                                        "ResponseType" : "mpf", 
                                                         "JetNumber" : 0}
        
        cutPipe["Consumer"]["bin_balance_response"] = { "Name" : "bin_response",
                                                        "ResponseType" : "bal",
                                                        "ProductName" : "balresp_" +  algoName,
                                                        "JetNumber" : 1 }
        cutPipe["Consumer"]["bin_balance_response_2ndJet"] = { "Name" : "bin_response",
                                                              "ResponseType" : "bal",
                                                        "ProductName" : "bal_jet2_z_" +  algoName,
                                                        "JetNumber" : 2 }


        # only add the nocut pipeline for the default ( no binning )

        #if name == "default":
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


def AddSingleCutConsumer( pline, cut_name, cut_id, algoname ):
    
    AddConsumer(pline, "cut_" + algoname + cut_name + "_overnpv", 
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "reco",
                  "ProductName" : "cut_" + algoname + cut_name + "_overnpv"})
    
    AddConsumer(pline, "cut_" + algoname + cut_name + "_overzpt", 
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "zpt",
                  "ProductName" : "cut_" + algoname + cut_name + "_overzpt"})

def AddCutConsumer( pipelineDict, algos):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if p == "default_" + algo:
                AddConsumer(pval, "cut_statistics",
                            { "Name": "cut_statistics" })
                AddConsumer(pval, "filter_statistics",
                            { "Name": "filter_statistics" })
                # for every intersting cut
                AddConsumer(pval, "cut_" + algo + "_all", 
                            { "Name" : "generic_profile_consumer",
                              "RunUnfiltered" : 1,
                              "YSource" : "cutvalue",
                              "CutId" : -1,
                              "XSource" : "reco",
                              "ProductName" : "cut_" + algo + "_all"})
                AddSingleCutConsumer(pval, "jet2pt_over_zpt", 16, algo )
                AddSingleCutConsumer(pval, "back_to_back", 32, algo )
                AddSingleCutConsumer(pval, "zmass", 64, algo )
                AddSingleCutConsumer(pval, "muon_pt", 2, algo )
              
def AddLumiConsumer( pipelineDict, algos):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if p == "default_" + algo:                
                AddConsumerEasy(pval,  
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "eventcount",
                              "XSource" : "intlumi",
                              "ProductName" : "eventcount_" + algo + "_lumi"})
        
                AddConsumerEasy(pval, 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "eventcount",
                              "XSource" : "runnumber",
                              "ProductName" : "eventcount_" + algo + "_runnumber"})


def AddHltConsumer( pipelineDict, algoNames, hlt_names):
    for algo in algoNames:
        for hname in hlt_names:
            for p, pval in pipelineDict["Pipelines"].items():
                if p == "default_" + algo + "nocuts":
                    AddConsumer(pval, "hlt_" + algo + "_" + hname + "_prescale", 
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname, 
                                                  "XSource" : "runnumber",
                                                  "ProductName" : "hlt_" + algo + "_" + hname + "_prescale"})


def ExpandPtBins( pipelineDict, ptbins, includeSource):
    newDict = dict()
        
    for name, elem in pipelineDict.items():
        # dont do this for uncut events
        if not "nocuts" in name:
            i = 0
            for upper in ptbins[1:]:
                ptbinsname =  "Bin" + str(ptbins[i]) + "To" + str(upper)
    
                newPipe = copy.deepcopy(elem)
                
                newPipe["Filter"].append( "ptbin")
                
                newPipe["FilterPtBinLow"] = ptbins[i]
                newPipe["FilterPtBinHigh"] = upper
    
                newDict[name + "_" + ptbinsname ] = newPipe
                i = i + 1

    if includeSource:
        return dict( pipelineDict.items() +  newDict.items() )
    else:
        return newDict

def AddCorrectionPlots( conf, algoNames, l3residual = False, level = 3 ):
    for algo in algoNames:
        for p, pval in conf["Pipelines"].items():
            if p == "default_" + algo:
                AddConsumer(pval, "L1_" + algo + "_npv", 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo,
                              "Jet2Ratio" : algo + "L1",
                              "XSource" : "reco",
                              "ProductName" : "L1_" + algo + "_npv"})
                if level > 1:
                    AddConsumer(pval, "L2_" + algo + "_jeteta", 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1",
                              "Jet2Ratio" : algo + "L1L2",
                              "XSource" : "jeteta",
                              "ProductName" : "L2_" + algo + "_jeteta"})
                if level > 2:
                    AddConsumer(pval, "L3_" + algo + "_jetpt", 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2",
                              "Jet2Ratio" : algo + "L1L2L3",
                              "XSource" : "jetpt",
                              "ProductName" : "L3_" + algo + "_jetpt"})
                    
                if l3residual:
                    AddConsumer(pval, "L3Res_" + algo + "_jetpt", 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2L3",
                              "Jet2Ratio" : algo + "L1L2L3Res",
                              "XSource" : "jetpt",
                              "ProductName" : "L3Res_" + algo + "_jetpt"})
                    AddConsumer(pval, "L3Res_" + algo + "_jeteta", 
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2L3",
                              "Jet2Ratio" : algo + "L1L2L3Res",
                              "XSource" : "jeteta",
                              "ProductName" : "L3Res_" + algo + "_jeteta"})
            
            
                
def ExpandDefaultMcConfig( ptBins, algoNames, conf_template, useFolders, FolderPrefix = ""):
    conf = copy.deepcopy(conf_template)

    # generate folder names
    srcFolder = []
    for i in range( len(ptBins) - 1):
        srcFolder += ["Pt" + str(ptBins[i]) + "to" + str(ptBins[i+1]) + "_incut"]

    algoPipelines = {}

    # generate pipelines for all algorithms
    for algo in algoNames:
        for p, pval in conf["Pipelines"].items():

            pline = copy.deepcopy( pval )
            pline["JetAlgorithm"] = algo
            algoPipelines[ p + "_" +  algo  ] = pline
            
    conf["Pipelines"] = algoPipelines

    #conf["Pipelines"]["default"]["CustomBins"] = ptBins
    conf["Pipelines"] = ExpandCutNoCut( conf["Pipelines"] )    

    # create pipelines for all bins
    conf["Pipelines"] = ExpandPtBins(  conf["Pipelines"], ptBins, True )

    #set the folder name
    for p, pval in conf["Pipelines"].items():
        ptVal = "NoBinning"

        if "ptbin" in pval["Filter"]:
            ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])

        if "incut" in pval["Filter"]:
            ptVal = ptVal + "_incut"
        else:
            ptVal = ptVal + "_allevents"

        pval["RootFileFolder"] = FolderPrefix + ptVal


    for algo in algoNames:
        # create second level pipelines
        pipename = FolderPrefix + "sec_default_" + algo
        
        secpline = {} 

        # code this in a more generic way
        secpline["Consumer"] = {}
        secpline["Consumer"]["bal_response"] = { "Name" : "response_balance",
                                             "SourceFolder" : srcFolder,
                                             "SourceResponse" : "balresp_" + algo,
                                             # this product will be in the upmost folder
                                             "ProductName"    : "balresp_" + algo,
                                             "SourceBinning"  : "z_pt_" + algo}
        
        secpline["Consumer"]["mpf_response"] = { "Name" : "response_balance",
                                             "SourceFolder" : srcFolder,
                                             "SourceResponse" : "mpfresp_" + algo,
                                             # this product will be in the upmost folder
                                            "ProductName"    : "mpfresp_" + algo,
                                             "SourceBinning"  : "z_pt_" + algo}      
        
        secpline["Level"] = 2
        #secLevelPline[FolderPrefix + "sec_default"]["CustomBins"] = ptBins
        secpline["SecondLevelFolderTemplate"] = FolderPrefix + "XXPT_BINXX_incut"
        secpline["RootFileFolder"] = FolderPrefix
        
        
        #for (key, val) in conf["Pipelines"].items():
        #    secLevelPline[ FolderPrefix + key ] = val
    
        conf["Pipelines"][pipename] =  secpline

    return conf

    
    
def ExpandDefaultDataConfig( ptBins, conf_template, useFolders, FolderPrefix = ""):
    conf = ExpandDefaultMcConfig( ptBins, conf_template, useFolders, FolderPrefix)

    #conf["Pipelines"][FolderPrefix + "default"]["AdditionalConsumer"].append( "event_storer" )

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
    subprocess.call(["./closure",filename])
    try:
        import pynotify
        if pynotify.init("CalibFW resp_cuts"):
            n = pynotify.Notification("CalibFW resp_cuts", "run with config " + filename + " done")
            n.show()
    except:
        pass

