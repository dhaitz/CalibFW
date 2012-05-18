import copy
import subprocess
import glob
import socket
#import argparse
import ConfigParser
import os.path
import stat
import getpass
import json

def CreateFileList(wildcardExpression, args=None):
    print "Creating file list from " + wildcardExpression
    inputfiles = glob.glob(wildcardExpression)

    if args is not None and len(args) > 1 and "fast" in args[1]:  # both 'fast' and '--fast' usable
        inputfiles = inputfiles[:2]
    return inputfiles

# Deprecated (CreateFileList does this)
def ApplyFast(inputfiles, args):
    if len(args) > 1 and args[1] == "fast":
        return inputfiles[:2]
    return inputfiles


def GetDefaultBinning():
    return [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]


def GetDataPath():
    hname = socket.gethostname()
    username = getpass.getuser()
    # feel free to insert your machine here !
    if username == 'berger':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/storage/6/berger/zpj/"
    elif username == 'dhaitz':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/storage/6/berger/zpj/"
    elif hname == "saturn":
        return "/home/poseidon/uni/data/Kappa/"
    elif "ekpcms" in hname:
        return "/storage/5/hauth/zpj/"
    elif "ekpplus" in hname:
        return "/storage/5/hauth/zpj/"
    else:
        print "Machine " + hname + " not found in ClosureConfigBase. Please insert it."
        exit(0)


def GetCMSSWPath():
    hname = socket.gethostname()
    username = getpass.getuser()
    # feel free to insert your machine here !
    if username == 'berger':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/storage/6/berger/CMSSW_4_2_8_patch1/"
    elif username == 'dhaitz':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/portal/ekpcms5/home/dhaitz/CMSSW_5_2_1/"
    elif hname == "saturn":
        return "/home/poseidon/uni/data/Kappa/"
    elif "ekpcms" in hname:
        return "/storage/5/hauth/zpj/CMSSW_4_2_8/"
    elif "ekpplus" in hname:
        return "/storage/5/hauth/zpj/CMSSW_4_2_8/"
    else:
        print "Machine " + hname + " not found in ClosureConfigBase. Please insert it."
        exit(0)


def GetBasePath():
    hname = socket.gethostname()
    # feel free to insert your machine here !
    username = getpass.getuser()
    if username == 'berger':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/storage/6/berger/zpj/CalibFW/"
    elif username == 'dhaitz':
        if 'ekpcms' in hname or 'ekpplus' in hname:
            return "/portal/ekpcms5/home/dhaitz/git/CalibFW/"    
    elif hname == "saturn":
        return "/home/poseidon/uni/data/Kappa/"
    elif hname == "ekpcms5":
        return "/storage/5/hauth/zpj/CalibFW/"
    elif hname == "ekpcms4.physik.uni-karlsruhe.de":
        return "/storage/5/hauth/zpj/CalibFW/"
    else:
        print "Machine " + hname + " not found in ClosureConfigBase. Please insert it."
        exit(0)


def getDefaultCorrectionL2(data_path):
    globalTag = "GR_R_44_V13_"

    names = ["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]
    algos = ["AK5PF", "AK7PF", "KT4PF", "KT6PF", "AK5Calo", "AK7Calo", "KT4Calo", "KT6Calo", "IC5PF", "IC5Calo"]
    g_l2_correction_data = [n + "Jets:" + data_path + "jec_data/" + globalTag + a + "_L2Relative.txt" for n, a in zip(names, algos)]

    return g_l2_correction_data


def addCHS(algorithms):
    """can be used as [algos =] addCHS(algos) or algos = addCHS([AK5, etc.])"""
    algorithms += [a.replace("PFJets", "PFJetsCHS") for a in algorithms
        if "PFJets" in a and "PFJetsCHS" not in a and a.replace("PFJets", "PFJetsCHS") not in algorithms]
    return algorithms


def getNewestJson(variant="PromptReco_Collisions12"):
    jsons = glob.glob('data/json/Cert*'+variant+'*.txt')
    jsons.sort()
    print "JSON:", jsons[-1]
    return jsons[-1]


def GetBaseConfig():
    d = dict()

    # the order of this producers is important
    d["GlobalProducer"] = ["valid_muon_producer", "z_producer",
                           "valid_jet_producer", "corr_jet_producer",
                           "jet_sorter_producer","pu_reweighting_producer"]

    d["ThreadCount"] = 1
    d["Pipelines"] = { "default": {
            "Level": 1,
            "JetAlgorithm" : "to_set",
            "RootFileFolder": "",

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

def GetVBFBaseConfig():
    d = {}

    # the order of this producers is important
    d["GlobalProducer"] = ["valid_muon_producer" , "z_producer",
                           "pu_reweighting_producer", "valid_jet_producer",
                           "corr_jet_producer", "jet_sorter_producer"]

    d["ThreadCount"] = 1
    d["Pipelines"] = { "default": {
            "Level": 1,
            "JetAlgorithm": "to_set",
            "RootFileFolder": "",

            "CutMuonEta": 2.4,
            "CutMuonPt": 15,
            "CutZMassWindow": 20,

            "CutLeadingJetPt": 30,
            "CutSecondJetPt": 30,
            "CutLeadingJetEta": 5.0,
            "CutSecondJetEta": 5.0,

            "CutRapidityGap": 4.0,
            "CutInvariantMass": 500,


            "Cuts": ["leadingjet_pt",
                     "secondjet_pt",
                     "leadingjet_eta",
                     "secondjet_eta",
                     "rapidity_gap",
                     "jet_mass",
                     "muon_pt",
                     "muon_eta",
                     "zmass_window",
                     ],
            "Filter":["valid_z", "valid_jet"],
            "Consumer": {}
                      }
            }
    AddConsumerNoConfig( d["Pipelines"]["default"], "quantities_all")

    return d


def ApplyPUReweighting(conf, dataset="kappa50_MC12_madgraph_190456-191859_8TeV_PromptReco", weightfile="data/pileup/puweights.json"):
    """Use pile-up reweighting

       This function turns the pile-up reweighting on and sets the corresponding
       entries in the configuration. The cross sections and weight factors are
       calculated via macros/weightCalc.py and stored in the following
       dictionary.
    """
    # The following dictionary stores the weights per dataset
    try:
        f = open(weightfile)
    except:
        print weightfile, "does not exist."
        print "Please provide this file or do not use ApplyPUReweighting."
        exit(0)
    try:
        d = json.load(f)
    except:
        print weightfile, "is no json file."
        print "Please provide a correct file or do not use ApplyPUReweighting."
        exit(0)
    f.close()

    if dataset not in d:
        print "No PU weights for this dataset:", dataset
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them with the weightCalc macro or do not use ApplyPUReweighting."
        exit(0)

    conf["EnablePuReweighting"] = 1
    conf["GlobalXSection"] = d[dataset]["xsection"]
    conf["RecovertWeight"] = d[dataset]["weights"] + [0.0]*(60 - len(d[dataset]["weights"]))
    return conf


def Apply2ndJetReweighting(conf, dataset='powhegFall11', method='reco'):
    """Use 2nd jet reweighting

       This function turns the 2nd jet reweighting on and sets the corresponding
       entries in the configuration. It is intended for powheg samples which
       need it. The weight factors are calculated via macros/weightCalc.py and
       stored in the following dictionary.
    """
    dataset += method
    d = {
        "powhegFall11reco" : #
            [0.940137088, 1.087205739, 1.288389912, 1.606806959, 1.930252398,
             2.385763303, 2.786953831, 3.165882435, 3.856122666, 4.291719589,
             4.726936824, 5.259861565, 6.976823094, 6.753129989, 8.956200281,
             8.735612201, 9.230755331, 13.84305761, 7.944236954, 11.64005401,
             10.80501920, 14.73771973, 18.54871784, 20.00000000, 20.00000000],
        }

    if dataset not in d:
        print "No 2nd jet weights for this dataset:", dataset
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them in ClosureConfigBase or do not use Apply2ndJetReweighting."
        exit(0)

    conf["Enable2ndJetReweighting"] = 1
    conf["2ndJetWeight"] = d[dataset] + [0.0]*(60 - len(d[dataset]))
    return conf


def GetMcBaseConfig(analysis='zjet', run='2011'):
    if analysis == 'vbf':
        d = GetVBFBaseConfig()
    else:
        d = GetBaseConfig()

    d["UseWeighting"] = 1
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0

    d["InputType"] = "mc"

    if run == '2011':
        d["JecBase"] = GetBasePath() + "data/jec_data/START44_V12_"
    elif run == '2012':
        d["JecBase"] = GetBasePath() + "data/jec_data/START44_V12_"
    else:
        print "MC period", run, "is undefined. No jet corrections known."
        exit(0)

    d["GlobalProducer"] += ["jet_matcher"]
    return d

def GetDefaultDataPipeline():
    pline = GetDataBaseConfig()["Pipelines"]["default"]

    pline["FilterInCutIgnored"] = 0
    pline["Filter"].append ("incut")

    return pline

def GetDataBaseConfig(analysis='zjet',run='2011'):
    if analysis == 'vbf':
        d = GetVBFBaseConfig()
    else:
        d = GetBaseConfig()
    if run == '2011':
        d["JecBase"] = GetBasePath() +  "data/jec_data/GR_R_44_V13_"
        d["JsonFile"] = GetBasePath() + "data/json/Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON.txt"
    elif run == '2012':
        d["JecBase"] = GetBasePath() +  "data/jec_data/GR_R_44_V13_"
        d["JsonFile"] = GetBasePath() + "data/json/Cert_190456-194076_8TeV_PromptReco_Collisions12_JSON.txt"
    else:
        print "Run period", run, "is undefined. No json and jet corrections known."
        exit(0)

    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["UseGlobalWeightBin"] = 0

    d["HltPaths"] = [
    # Mu7 Trigger
    "HLT_DoubleMu7_v1", "HLT_DoubleMu7_v2", "HLT_DoubleMu7_v3", "HLT_DoubleMu7_v4", "HLT_DoubleMu7_v5",
    # Mu8 Trigger
    "HTL_Mu8_v16",
    # Mu13_Mu8 Trigger
    "HLT_Mu13_Mu8_v1", "HLT_Mu13_Mu8_v2", "HLT_Mu13_Mu8_v3", "HLT_Mu13_Mu8_v4", "HLT_Mu13_Mu8_v5",
    "HLT_Mu13_Mu8_v6", "HLT_Mu13_Mu8_v7", "HLT_Mu13_Mu8_v8", "HLT_Mu13_Mu8_v9", "HLT_Mu13_Mu8_v10",
    "HLT_Mu13_Mu8_v11", "HLT_Mu13_Mu8_v12", "HLT_Mu13_Mu8_v13", "HLT_Mu13_Mu8_v14", "HLT_Mu13_Mu8_v15",
    # Mu17_Mu8 Trigger
    "HLT_Mu17_Mu8_v1", "HLT_Mu17_Mu8_v2", "HLT_Mu17_Mu8_v3", "HLT_Mu17_Mu8_v4", "HLT_Mu17_Mu8_v5",
    "HLT_Mu17_Mu8_v6", "HLT_Mu17_Mu8_v7", "HLT_Mu17_Mu8_v8", "HLT_Mu17_Mu8_v9", "HLT_Mu17_Mu8_v10",
    "HLT_Mu17_Mu8_v11", "HLT_Mu17_Mu8_v12", "HLT_Mu17_Mu8_v13", "HLT_Mu17_Mu8_v14", "HLT_Mu17_Mu8_v15",
    "HLT_Mu17_Mu8_v16", "HLT_Mu17_Mu8_v17", "HLT_Mu17_Mu8_v18"
    ]

    d["InputType"] = "data"
    d["Pipelines"]["default"]["Filter"].append ("json")
    d["Pipelines"]["default"]["Filter"].append ("hlt")

    d["GlobalProducer"]+= ["hlt_selector"]

    #for key, val in d["Pipelines"].items():
      #  "Filter":["valid_z", "valid_jet"]
       # val["Cuts"].append( "hlt" )
       # val["Cuts"].append( "json" )

    return d


def ExpandRange(pipelineDict, varName, vals, setRootFolder=True, includeSource=True, alsoForNoCuts=False, correction="L1L2L3", onlyBasicQuantities=True):
    newDict = dict()

    for name, elem in pipelineDict.items():

        if (elem["Level"] == 1 )and ( ( not "nocuts" in name) or alsoForNoCuts ) and ( correction in name):
            for v in vals:
                newPipe = copy.deepcopy(elem)
                newPipe[ varName ] = v

                #only do basic plots
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic( newPipe )

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
                 includeSource=False, onlyOnIncut=True,
                 onlyBasicQuantities = True):
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

                #only do basic plots
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic( newpipe )
                if filtername == "JetEta":
                    newpipe["CutLeadingJetEta"] = 5.0

                #print(new_pipe)
                newpipe["Filter"].append(filtername.lower())
                newpipe["Filter" + filtername + "Low"] = l
                newpipe["Filter" + filtername + "High"] = h
                f = foldername.format(name=filtername, low=l, high=h)
                f = "_" + f.replace(".", "_")

                newName = pipeline + f
                newRootFileFolder = newpipe["RootFileFolder"] + f
                newDict[newName] = newpipe
                if foldername is not None:
                    newDict[newName]["RootFileFolder"] = newRootFileFolder
    if includeSource:
        return dict(pipelines.items() +  newDict.items())
    else:
        return newDict


def ExpandRange2Cut(pipelines, cutname, low, high=None,
                foldername="var_{name}_{low}to{high}",
                includeSource=True, onlyOnIncut=True,
                onlyBasicQuantities = True):
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

                #only do basic plots
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic( newpipe )

                #print(new_pipe)
                newpipe["Cut" + cutname.replace("_", "") + "Low"] = l
                newpipe["Cut" + cutname.replace("_", "") + "High"] = h
                f = foldername.format(name=cutname, low=l, high=h)
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

    AddConsumer(pline, "cut_" + cut_name + "_npv_" + algoname,
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "reco",
                  "ProductName" : "cut_" + cut_name + "_npv_" + algoname })

    AddConsumer(pline, "cut_" + cut_name + "_zpt_" + algoname,
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "zpt",
                  "ProductName" : "cut_" + cut_name + "_zpt_" + algoname})

def AddCutConsumer( pipelineDict, algos):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if p == "default_" + algo:
                AddConsumer(pval, "cut_statistics",
                            { "Name": "cut_statistics" })
                AddConsumer(pval, "filter_statistics",
                            { "Name": "filter_statistics" })
                # for every intersting cut
                AddConsumer(pval, "cut_all_npv_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "RunUnfiltered" : 1,
                              "YSource" : "cutvalue",
                              "CutId" : -1,
                              "XSource" : "reco",
                              "ProductName" : "cut_all_npv_" + algo})
                AddSingleCutConsumer(pval, "jet2pt", 16, algo )
                AddSingleCutConsumer(pval, "backtoback", 32, algo )
                AddSingleCutConsumer(pval, "zmass", 64, algo )
                AddSingleCutConsumer(pval, "muonpt", 2, algo )

def AddLumiConsumer( pipelineDict, algos):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if p == "default_" + algo:
                AddConsumerEasy(pval,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "eventcount",
                              "XSource" : "intlumi",
                              "ProductName" : "eventcount_lumi_" + algo})

                AddConsumerEasy(pval,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "eventcount",
                              "XSource" : "runnumber",
                              "ProductName" : "eventcount_run_" + algo})


def AddHltConsumer( pipelineDict, algoNames, hlt_names):
    for algo in algoNames:
        for hname in hlt_names:
            for p, pval in pipelineDict["Pipelines"].items():
                #print p
                if p == "default_" + algo + "nocuts":
                    #print "ADDING"
                    AddConsumer(pval, "hlt_" + hname + "_prescale_run_" + algo,
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "runnumber",
                                                  "ProductName" : "hlt_" + hname + "_prescale_run_" + algo})
                    AddConsumer(pval, "hlt_" + hname + "_prescale_lumi" + algo,
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "intlumi",
                                                  "ProductName" : "hlt_" + hname + "_prescale_lumi_" + algo})
    # plot the selcted hlt
    for algo in algoNames:
        for p, pval in pipelineDict["Pipelines"].items():
            #print p
            if p == "default_" + algo + "nocuts":
                AddConsumer(pval, "hlt_selected_prescale_lumi" + algo,
                                            { "Name" : "generic_profile_consumer",
                                              "YSource" : "selectedhltprescale",
                                              "YSourceConfig" : hname,
                                              "XSource" : "intlumi",
                                              "ProductName" : "hlt_selected_prescale_lumi" + algo})
                AddConsumer(pval, "hlt_selected_prescale_run" + algo,
                                            { "Name" : "generic_profile_consumer",
                                              "YSource" : "selectedhltprescale",
                                              "YSourceConfig" : hname,
                                              "XSource" : "runnumber",
                                              "ProductName" : "hlt_selected_prescale_run" + algo})


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
                AddConsumer(pval, "L1_npv_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo,
                              "Jet2Ratio" : algo + "L1",
                              "XSource" : "reco",
                              "ProductName" : "L1_npv_" + algo})
                if level > 1:
                    AddConsumer(pval, "L2_jeteta_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1",
                              "Jet2Ratio" : algo + "L1L2",
                              "XSource" : "jeteta",
                              "ProductName" : "L2_jeteta_" + algo})
                if level > 2:
                    AddConsumer(pval, "L3_jetpt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2",
                              "Jet2Ratio" : algo + "L1L2L3",
                              "XSource" : "jetpt",
                              "ProductName" : "L3_jetpt_" + algo})

                if l3residual:
                    AddConsumer(pval, "L3Res_jetpt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2L3",
                              "Jet2Ratio" : algo + "L1L2L3Res",
                              "XSource" : "jetpt",
                              "ProductName" : "L3Res_jetpt_" + algo})
                    AddConsumer(pval, "L3Res_jeteta_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio",
                              "Jet1Ratio" : algo + "L1L2L3",
                              "Jet2Ratio" : algo + "L1L2L3Res",
                              "XSource" : "jeteta",
                              "ProductName" : "L3Res_jeteta_" + algo})


def ReplaceWithQuantitiesBasic(pline):
    RemoveConsumer( pline, "quantities_all" )
    AddConsumerNoConfig(pline, "quantities_basic")


def ExpandDefaultMcConfig(algoNames, conf_template, useFolders=True, FolderPrefix="", binning=GetDefaultBinning() ):
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



def ExpandDefaultDataConfig(algoNames, conf_template, useFolders=True, FolderPrefix="", binning=GetDefaultBinning()):
    conf = ExpandDefaultMcConfig(algoNames, conf_template, useFolders, FolderPrefix, binning)
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
    print "Configured", len(settings["Pipelines"]), "Pipelines"


def StoreGCDataset( settings, nickname, filename ):
    print "Generating " + filename

    # ordering is important in the .dbs file format
    cfile = open(filename, 'wb')
    cfile.write ("[" +  nickname + "]\n" )
    cfile.write ("nickname = " +  nickname + "\n" )
    cfile.write ("events = " +  str ( - len ( settings["InputFiles"] ))  + "\n" )

    path = os.path.split(  settings["InputFiles"][0] ) [0]
    cfile.write ("prefix = " + path + "\n" )

    for f in settings["InputFiles"]:
        cfile.write ( os.path.split(f)[1] +  ' = -1\n' )

    cfile.close()

def StoreGCConfig ( settings, nickname, filename ):
    print "Generating " + filename

    config = ConfigParser.RawConfigParser()
    # important, so the case is preserved
    config.optionxform = str
    config.add_section("global")
    config.set("global", "include", "gc_common.conf")
    config.set("global", "workdir space", "0")
    config.add_section("UserMod")
    config.set("UserMod", "dataset", nickname + " : " + nickname + ".dbs" )

    # Writing our configuration file to 'example.cfg'
    cfile = open(filename, 'wb')
    config.write(cfile)
    cfile.close()

def StoreGCCommon(settings, nickname, filename, output_folder):
    print "Generating " + filename

    config = ConfigParser.RawConfigParser()
    # important, so the case is preserved
    config.optionxform = str
    config.add_section("global")
    config.set("global", "module", "UserMod")
    config.set("global", "backend", "local")

    config.add_section("jobs")
    config.set("jobs", "in queue", 50)
    config.set("jobs", "shuffle", True)
    config.set("jobs", "wall time", "0:30:00" )
    config.set("jobs", "monitor", "scripts" )

    config.add_section("local")
    config.set("local", "queue", "short")

    config.add_section("UserMod")

    # we can run more data files with one MC job as they don't contain that many
    # events
    if settings["InputType"] == "mc":
        config.set("UserMod", "files per job", 3 )
    else:
        config.set("UserMod", "files per job", 20 )

    config.set("UserMod", "executable", "gc-run-closure.sh" )
    config.set("UserMod", "subst files", "gc-run-closure.sh" )
    #config.set("UserMod", "input files", "/usr/lib64/libboost_regex.so.2" )
    if "CMSSW_4_" in GetCMSSWPath():
        config.set("UserMod", "input files", "/wlcg/sw/cms/experimental/slc5_amd64_gcc434/cms/cmssw/CMSSW_4_2_8/external/slc5_amd64_gcc434/lib/libboost_regex.so.1.44.0")
    elif "CMSSW_5_" in GetCMSSWPath():
        config.set("UserMod", "input files", "/wlcg/sw/cms/slc5_amd64_gcc462/cms/cmssw/CMSSW_5_2_1/external/slc5_amd64_gcc462/lib/libboost_regex.so.1.47.0")
    else:
        print "I try to use boost 1.44 as before. This could fail if linked against newer CMSSW versions."
        config.set("UserMod", "input files", "/wlcg/sw/cms/experimental/slc5_amd64_gcc434/cms/cmssw/CMSSW_4_2_8/external/slc5_amd64_gcc434/lib/libboost_regex.so.1.44.0")

    config.add_section("storage")
    config.set("storage", "se path", "dir://" + output_folder )
    config.set("storage", "se output files", settings["OutputPath"] + ".root" )
    config.set("storage", "se output pattern", "@NICK@_job_@MY_JOBID@.root" )

    # Writing our configuration file to 'example.cfg'
    cfile = open(filename, 'wb')
    config.write(cfile)
    cfile.close()

def StoreMergeScript ( settings, nickname, filename, output_folder ):
    print "Generating " + filename

    cfile = open(filename, 'wb')
    cfile.write("hadd " + output_folder + settings["OutputPath"] + ".root " + output_folder + nickname + "_job_*.root\n" )
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)

def StoreShellRunner ( settings, nickname, filename ):
    print "Generating " + filename
    cfile = open(filename, 'wb')
    cfile.write("echo $FILE_NAMES\n")
    cfile.write("cd " + GetCMSSWPath() +"\n")
    if "CMSSW_5_" in GetCMSSWPath():
        cfile.write("export SCRAM_ARCH=slc5_amd64_gcc462\n")
        cfile.write("source /wlcg/sw/cms/cmsset_default.sh\n")
    else:
        cfile.write("source /wlcg/sw/cms/experimental/cmsset_default.sh\n")
    cfile.write("eval `scram runtime -sh`\n")
    cfile.write("cd -\n")
    cfile.write("source "+ GetBasePath() + "/scripts/CalibFWenv.sh\n")
    cfile.write( GetBasePath() + "closure " + GetBasePath() + "cfg/closure/" + nickname + ".py.json" )
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)


def Run(settings, arguments):

    # parser = argparse.ArgumentParser(description='Run the ROCED scheduler')
    #parser.add_argument('--batch',  help="Generate all Grid-Control configs", action='store_true' )
#    parser.add_argument('config', metavar='config file', nargs=1,
#                        help='Run using config file')
  #  parser.add_argument('--config', nargs=1, help="Run using config file" )

    #args = parser.parse_args(arguments)
    #print args
    filename = arguments[0] + ".json"
    StoreSettings( settings, filename)

    base_path = GetBasePath()
    print "BASEPATH", base_path

    if len(arguments) > 1 and "--storeonly" in arguments:
        "The settings were stored to", filename
        exit(0)

    if len(arguments) <= 1 or "batch" not in arguments[1]:  # both 'batch' and '--batch' usable
        subprocess.call(["./closure",filename])
    else:
        nickname = os.path.split(filename)[1]
        nickname = nickname.split(".")[0]
        print "Generating GC configs with nickname " + nickname + " ..."
        # store the input files in gc format
        if not os.path.exists( base_path + "work/" ) :
            os.mkdir( base_path + "work/" )
        if not os.path.exists( base_path + "work/" + nickname ) :
            os.mkdir( base_path + "work/" + nickname )
        if not os.path.exists( base_path + "work/" + nickname + "/out/" ) :
            os.mkdir( base_path + "work/" + nickname + "/out/" )

        StoreGCDataset(settings, nickname, base_path + "work/" + nickname + "/" + nickname + ".dbs")
        StoreGCConfig(settings, nickname, base_path + "work/" + nickname + "/" + nickname + ".conf")
        StoreGCCommon(settings, nickname, base_path + "work/" + nickname + "/gc_common.conf", base_path + "work/" + nickname + "/out/")
        StoreMergeScript(settings, nickname, base_path + "work/" + nickname + "/merge.sh", base_path + "work/" + nickname + "/out/")
        StoreShellRunner(settings, nickname, base_path + "work/" + nickname + "/gc-run-closure.sh")

        # generate merge script
        print "done"
    try:
        import pynotify
        if pynotify.init("CalibFW resp_cuts"):
            n = pynotify.Notification("CalibFW resp_cuts", "run with config " + filename + " done")
            n.show()
    except:
        pass

