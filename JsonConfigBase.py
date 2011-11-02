import copy
import subprocess


def getDefaultCorrectionL2(data_path="data/", globalTag="GR_R_42_V2"):
    data_path += "jec_data/" + globalTag + "_"
    g_l2_correction_data = [
      "ak5PFJets:" + data_path + "AK5PF_L2Relative.txt",
      "ak7PFJets:" + data_path + "AK7PF_L2Relative.txt",
      "kt4PFJets:" + data_path + "KT4PF_L2Relative.txt",
      "kt6PFJets:" + data_path + "KT6PF_L2Relative.txt",
      "ak5CaloJetst:" + data_path + "AK5Calo_L2Relative.txt",
      "ak7CaloJets:" + data_path + "AK7Calo_L2Relative.txt",
      "kt4CaloJets:" + data_path + "KT4Calo_L2Relative.txt",
      "kt6CaloJets:" + data_path + "KT6Calo_L2Relative.txt",
      "iterativeCone5PFJets:" + data_path + "IC5PF_L2Relative.txt",
      "iterativeCone5CaloJets:" + data_path + "IC5PF_L2Relative.txt",
    ]
    return g_l2_correction_data


def GetGitInformation():
    gitlog = subprocess.Popen("git --no-pager log -n 1 | head -n 1",
            stdout=subprocess.PIPE, shell=True).stdout.read()
    gitremote = subprocess.Popen("git remote -v",
            stdout=subprocess.PIPE, shell=True).stdout.read()
    gitlog = gitlog.replace("'", "`")
    gitremote = gitremote.replace("'", "`")
    return gitlog + "\n" + gitremote


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
            "CutSecondLeadingToZPtJet2Threshold" : 0.0,
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

# Keep a while for backwards compatibility
def ApplyReweightingSummer11May10ReReco(conf):
    ApplyReweighting(conf, 'summer11pythia')

def ApplyReweighting(conf, mcSample='summer11pythia'):
    conf["UseWeighting"] = 1
    conf["UseEventWeight"] = 1
    conf["UseGlobalWeightBin"] = 1
    conf["EventReweighting"] = 1
    if mcSample=='summer11pythia':
        conf["GlobalXSection"] = 1300.0
        conf["RecovertWeight"] = [0.084106611, 0.359957396, 0.918527617,
          1.351072773, 1.958318154, 1.820838641, 1.652629448, 1.588008618,
          1.364080640, 0.976496000, 0.738475795, 0.564293640, 0.423859827,
          0.355089079, 0.247419634, 0.236049025, 0.190887375, 0.149816132,
          0.211622601, 0.102733603, 0.078282562, 0.307589609, 0.000000000,
          0.059026297, 0.000000000, 0.000000000, 0.000000000, 0.000000000,
          0.000000000, 0.000000000, 0.000000000, 0.000000000, 0.000000000,
          0.000000000, 0.000000000, 0.000000000]

    elif mcSample == 'summer11powheg':
        conf["GlobalXSection"] = 1626.0
        conf["RecovertWeight"] = [0.057690346, 0.418533840, 0.859409910,
          1.396892036, 1.792193588, 1.971017274, 1.931228590, 1.740569189,
          1.453485495, 1.167719564, 0.900403570, 0.682236352, 0.504883700,
          0.363160202, 0.259842921, 0.182585931, 0.126762654, 0.085326813,
          0.058861369, 0.038883483, 0.025858987, 0.016399080, 0.010888594,
          0.006946350, 0.004449118, 0.002725434, 0.000000000, 0.000000000,
          0.000000000, 0.000000000, 0.000000000, 0.000000000, 0.000000000,
          0.000000000, 0.000000000, 0.000000000]

    else:
        print "No weights found for " + mcSample + ", reweighting is turned off."
        conf["GlobalXSection"] = 1600.0
        conf["EventReweighting"] = 0
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

    # json merged by compareJSON.py --or
    d["JsonFile"] = "data/json/Cert_160404-173692_7TeV_Run2011A_Collisions11_JSON.txt"
    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0
    d["InputType"] = "data"
    d["Pipelines"]["default"]["CutHLT"] = "DoubleMu"
    
    for key, val in d["Pipelines"].items():
        val["Cuts"].append( "hlt" ) 
        val["Cuts"].append( "json" )

    return d


def CreateEndcapPipelines ( curPipelines ):
    endcap = {}
    for (k,v) in curPipelines.items():
        if ( "incut" in v["RootFileFolder"] ) and ( v["Level"] == 1 ):
            newp = copy.deepcopy( v )
            newp["CutLeadingJetEta"] = 2.4
            newp["Filter"].append("jeteta")
            newp["FilterJetEtaLow"] = 1.5
            newp["FilterJetEtaHigh"] = 2.4
            newp["RootFileFolder"] = "endcap_" + newp["RootFileFolder"]
            endcap[ "endcap" + k ] = newp
    
    return endcap


def ExpandRange(pipelineDict, varName, vals, setRootFolder=True, includeSource=True, onlyOnIncut=True):
    newDict = dict()

    for name, elem in pipelineDict.items():
        
        if elem["Level"] == 1:
	    if (not onlyOnIncut) or ( "incut" in elem["RootFileFolder"] ):  
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

def ExpandRange2(pipelines, filtername, low, high=None,
                 foldername="var_{name}_{low}to{high}",
                 includeSource=True, onlyOnIncut=True):
    """Add pipelines with values between low and high for filtername

    This only works if the filter is lowercase and it uses two variables
    called Filter<FilterName>Low/High
    The foldername string can contain the variables {name}, {low} and {high}
    """
    newDict = {}
    for pipeline, subdict in pipelines.items():
        if subdict["Level"] == 1 and (not onlyOnIncut or
                "incut" in subdict["RootFileFolder"]):
            for l, h in zip(low, high):
                # copy existing pipeline (subdict) and modify it
                newpipe = copy.deepcopy(subdict)
                #print(new_pipe)
                newpipe["Filter"].append(filtername.lower())
                newpipe["Filter" + filtername + "Low"] = l
                newpipe["Filter" + filtername + "High"] = h
                f = foldername.format(name=filtername, low=l, high=h}
                f = "_" + f.replace(".", "_")
                newName = pipeline + f
                newRootFileFolder =  newpipe["RootFileFolder"] + f
                newDict[newName] = newpipe
                if foldername is not None:
                    newDict[newName]["RootFileFolder"] = newRootFileFolder
    if includeSource:
        return dict(pipelines.items() +  newDict.items())
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
    # jet eta 8
    
    # cuts to ignore 
    # 2nd Jet Pt          16
    # muon pt cut         2
    # back to back cut    32 
    # zmass 64

    
    # bitmask :  0011 1010 = 50

   pline_qualitycuts = copy.deepcopy( conf["Pipelines"]["default"] )
   pline_qualitycuts["FilterInCutIgnored"] = 58        
   conf["Pipelines"]["NoBinning_qualitycuts"] = pline_qualitycuts
   
   return

    
def ExpandDefaultMcConfig( ptBins, conf_template, useFolders=True, FolderPrefix = ""):
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

    # add quality cuts pipeline
    pline_qualitycuts = copy.deepcopy( conf["Pipelines"]["default"] )
    pline_qualitycuts["FilterInCutIgnored"] = 50 
    pline_qualitycuts["RootFileFolder"] = "NoBinning_qualitycuts"

    conf["Pipelines"]["NoBinning_qualitycuts"] = pline_qualitycuts


    return conf

    
    
def ExpandDefaultDataConfig( ptBins, conf_template, useFolders=True, FolderPrefix = ""):
    conf = ExpandDefaultMcConfig( ptBins, conf_template, useFolders, FolderPrefix)

    # dont use the event storer for now. enable if you have concrete need for this
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
        f.write(jsonOut)
        print "No json Module found. Using fallback method ..."
        
    f.close()
    
    print "Configured", len(settings["Pipelines"]), "Pipelines"
    
    
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

