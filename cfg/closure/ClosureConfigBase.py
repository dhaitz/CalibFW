import copy
import subprocess
import glob
import socket

def CreateFileList( wildcardExpression):
    flist = []

    print "Creating file list from " + wildcardExpression

    for name in glob.glob(wildcardExpression):
        flist.append(name)

    return flist


def GetDefaultBinning():
    return [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]

def GetDataPath():

    hname = socket.gethostname()
    # feel free to insert your machine here !
    if hname == "saturn":
        return "/home/poseidon/uni/data/Kappa/"
    elif hname == "ekpcms5":
        return "/storage/5/hauth/zpj/"
    else:
        print "Machine " + hname + " not found in ClosureConfigBase. Please insert it."
	exit(0)

# only leaves the first 5 root files, for a faster processing while testing
def ApplyFast( inputfiles, args ):
    if len(args) > 1:
        if args[1] == "fast":
            inputfiles = inputfiles[:2]

    return inputfiles


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

    # the order of this producers is important
    d["GlobalProducer"] = ["valid_muon_producer" , "z_producer",
                           "pu_reweighting_producer", "valid_jet_producer",
                           "corr_jet_producer", "jet_sorter_producer"]

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
    AddConsumerNoConfig( d["Pipelines"]["default"], "quantities_all")

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

    d["HltPaths"] = ["HLT_DoubleMu7_v1", "HLT_DoubleMu7_v2", "HLT_DoubleMu7_v8", "HLT_Mu13_Mu8_v2", "HLT_Mu17_Mu8_v2", "HLT_Mu17_Mu8_v3","HLT_Mu17_Mu8_v4", "HLT_Mu17_Mu8_v6", "HLT_Mu17_Mu8_v7", "HLT_Mu17_Mu8_v10", "HLT_Mu17_Mu8_v11" ]

    d["InputType"] = "data"
    d["Pipelines"]["default"]["Filter"].append ("json")
    d["Pipelines"]["default"]["Filter"].append ("hlt")

    d["JecBase"] = "data/jec_data/GR_R_42_V19_"

    d["GlobalProducer"]+= ["hlt_selector"]

    #for key, val in d["Pipelines"].items():
      #  "Filter":["valid_z", "valid_jet"]
       # val["Cuts"].append( "hlt" )
       # val["Cuts"].append( "json" )

    return d


def ExpandRange( pipelineDict, varName, vals, setRootFolder, includeSource, alsoForNoCuts = False, correction = "L1L2L3"):
    newDict = dict()

    for name, elem in pipelineDict.items():

        if (elem["Level"] == 1 )and ( ( not "nocuts" in name) or alsoForNoCuts ) and ( correction in name):
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

def AddMetaDataProducer( pline, name, config):
    pline["MetaDataProducer"][name] = config

def AddConsumerEasy( pline, consumer):
    pline["Consumer"][ consumer["ProductName"] ] = consumer

def AddConsumerNoConfig( pline, consumer_name):
    cons_dict = { "Name" : consumer_name }
    pline["Consumer"][ consumer_name ] = cons_dict

def RemoveConsumer( pline, consumer_name):
    if consumer_name in pline["Consumer"] :
        del pline["Consumer"][ consumer_name ]

def AddMetaDataProducerEasy( pline, producer_name):
    pline["MetaDataProducer"][ consumer["Name"] ] = producer_name


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
                #print p
                if p == "default_" + algo + "nocuts":
                    #print "ADDING"
                    AddConsumer(pval, "hlt_" + algo + "_" + hname + "_prescale_runnumber",
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "runnumber",
                                                  "ProductName" : "hlt_" + algo + "_" + hname + "_prescale"})
                    AddConsumer(pval, "hlt_" + algo + "_" + hname + "_prescale_lumi",
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "intlumi",
                                                  "ProductName" : "hlt_" + algo + "_" + hname + "_prescale_lumi"})
    # plot the selcted hlt
    for algo in algoNames:
	for p, pval in pipelineDict["Pipelines"].items():
	    #print p
	    if p == "default_" + algo + "nocuts":
		AddConsumer(pval, "hlt_" + algo + "_selected_prescale_lumi",
					    { "Name" : "generic_profile_consumer",
					      "YSource" : "selectedhltprescale",
					      "YSourceConfig" : hname,
					      "XSource" : "intlumi",
					      "ProductName" : "hlt_" + algo + "_selected_prescale_lumi"})
		AddConsumer(pval, "hlt_" + algo + "_selected_prescale_runnumber",
					    { "Name" : "generic_profile_consumer",
					      "YSource" : "selectedhltprescale",
					      "YSourceConfig" : hname,
					      "XSource" : "runnumber",
					      "ProductName" : "hlt_" + algo + "_selected_prescale_runnumber"})


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


def ReplaceWithQuantitiesBasic(pline):
    RemoveConsumer( pline, "quantities_all" )
    AddConsumerNoConfig(pline, "quantities_basic")

def ExpandDefaultMcConfig(  algoNames, conf_template, useFolders, FolderPrefix = "", binning = GetDefaultBinning() ):
    conf = copy.deepcopy(conf_template)

    # generate folder names
    srcFolder = []
    for i in range( len(binning) - 1):
        srcFolder += ["Pt" + str(binning[i]) + "to" + str(binning[i+1]) + "_incut"]

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
    conf["Pipelines"] = ExpandPtBins(  conf["Pipelines"], binning, True )

    #set the folder name
    for p, pval in conf["Pipelines"].items():
        ptVal = "NoBinning"

        if "ptbin" in pval["Filter"]:
            ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])
            ReplaceWithQuantitiesBasic ( pval )

        if "incut" in pval["Filter"]:
            ptVal = ptVal + "_incut"

            if not ptVal == "NoBinning_incut":
                ReplaceWithQuantitiesBasic ( pval )
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



def ExpandDefaultDataConfig( conf_template, useFolders, FolderPrefix = ""):
    conf = ExpandDefaultMcConfig( conf_template, useFolders, FolderPrefix)

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

