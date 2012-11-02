#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""ClosureConfigBase provides the tools to make a valid closure config.

The most used functions are:
  - BaseConfig to generate a default configuration
  - CreateFileList to create a list of input files
  - Run to acutally call closure and run it
"""
import copy
import subprocess
import glob
import socket
import ConfigParser
import os
import stat
import getpass
import json

def CreateFileList(wildcardExpression, args=None):
    print "Creating file list from " + wildcardExpression
    inputfiles = glob.glob(wildcardExpression)

    if args is not None and len(args) > 1 and "fast" in args[1]:  # both 'fast' and '--fast' usable
        try:
            inputfiles = inputfiles[-int(args[2]):]
        except:
            inputfiles = inputfiles[-3:]
    return inputfiles


def GetDefaultBinning():
    return [30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]


def GetCMSSWPath(variable='CMSSW_BASE'):
    """Return the path of the sourced CMSSW release."""
    try:
        return os.environ[variable] + "/"
    except:
        print variable, "is not in shell variables:", os.environ.keys()
        print "Please source CMSSW!"
        exit(1)


def GetBasePath(variable='CLOSURE_BASE'):
    """Return the path of the closure repository (CalibFW)."""
    try:
        return os.environ[variable] + "/"
    except:
        print variable, "is not in shell variables:", os.environ.keys()
        print "Please source scripts/ClosureEnv.sh!"
        exit(1)


def GetWorkPath():
    """Return work path if the shell variable 'CLOSURE_WORK' is set."""
    try:
        return os.environ['CLOSURE_WORK'] + "/"
    except:
        print "WorkPath is not set. BasePath is used instead."
        return GetBasePath()


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


def BaseConfig(inputtype, run='2012', analysis='zjet'):
    """Basic configuration for closure.

    Return a default configuration for closure depending on

      - @param inputtype can be either 'data' or 'mc'. Default settings are adapted.
      - @param run can be either '2011' or '2012'. Parameters are set accordingly.
      - @param analysis can be either 'zjet' or 'vbf'. The cuts are set accordingly.
    """
    config = {
        'GlobalAlgorithms': [],
        'GlobalProducer': [    # The order of these producers is important!
            'valid_muon_producer', 'z_producer', 'valid_jet_producer',
            'corr_jet_producer', 'jet_sorter_producer','pu_reweighting_producer'
        ],
        'L1Correction': 'L1FastJet',
        'EnableMetPhiCorrection': False,
        'EnablePuReweighting': False,
        'Enable2ndJetReweighting': False,
        'Jec': "default",
        'JsonFile': "default",
        'InputFiles': [],
        'OutputPath': "closure_" + inputtype + "_" + run,
        'MuonID2011': (run=='2011'),
        'Pipelines': {
            'default': {
                'Level': 1,
                'JetAlgorithm': "default",
                'RootFileFolder': "",
                'Consumer': {},
            }
        },
        'InputType': inputtype,
    }

    config['Pipelines']['default'].update(GetCuts(analysis))

    if inputtype == 'data':
        config = SetDataSpecific(config, run)
    elif inputtype == 'mc':
        config = SetMcSpecific(config, run)
    else:
        print "The inputtype must be either 'data' or 'mc'."
        exit(1)

    AddConsumerNoConfig(config['Pipelines']['default'], 'quantities_all')
    return config


def SetMcSpecific(cfg, run='2012'):
    """Add Monte-Carlo specific settings to a config.

    The MC settings include

      - the set of Jet Energy Corrections
      - pile-up reweighting factors
      - additional producers
    """
    if run == '2011':
        cfg['Jec'] = GetBasePath() + "data/jec_data/START44_V12"
        #ApplyPUReweighting(cfg, '...')
    elif run == '2012':
        cfg['Jec'] = GetBasePath() + "data/jec_data/START52_V11"
        #ApplyPUReweighting(cfg, '...')
    else:
        print "MC period", run, "is undefined. No jet corrections known."
        exit(0)

    cfg['GlobalProducer'] += ['jet_matcher',
        'gen_producer', 'gen_balance_producer', 'gen_dibalance_producer']
    return cfg


def SetDataSpecific(cfg, run='2012'):
    """Add data specific settings to a config

    The data settings include

      - the set of Jet Energy Corrections
      - json files
      - HLT paths
      - additional producers
    """
    d = {}
    if run == '2011':
        cfg['Jec'] = GetBasePath() + "data/jec_data/GR_R_44_V13"
        cfg['JsonFile'] = GetBasePath() + "data/json/Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON_v2.txt"
        cfg['HltPaths'] = [
            # Mu7 Trigger
            "HLT_DoubleMu7_v1", "HLT_DoubleMu7_v2", "HLT_DoubleMu7_v3", "HLT_DoubleMu7_v4", "HLT_DoubleMu7_v5",
            # Mu8 Trigger
            "HTL_Mu8_v16",
            # Mu13_Mu8 Trigger
            "HLT_Mu13_Mu8_v1", "HLT_Mu13_Mu8_v2", "HLT_Mu13_Mu8_v3", "HLT_Mu13_Mu8_v4", "HLT_Mu13_Mu8_v5",
            "HLT_Mu13_Mu8_v6", "HLT_Mu13_Mu8_v7", "HLT_Mu13_Mu8_v8", "HLT_Mu13_Mu8_v9", "HLT_Mu13_Mu8_v10",
            "HLT_Mu13_Mu8_v11", "HLT_Mu13_Mu8_v12", "HLT_Mu13_Mu8_v13", "HLT_Mu13_Mu8_v14",
            ]
    elif run == '2012':
        cfg['Jec'] = GetBasePath() + "data/jec_data/GR_R_52_V9"
        cfg['JsonFile'] = GetBasePath() + "data/json/Cert_190456-201678_8TeV_PromptReco_Collisions12_JSON.txt"
        cfg['HltPaths'] = [
            "HLT_Mu17_Mu8_v1", "HLT_Mu17_Mu8_v2", "HLT_Mu17_Mu8_v3", "HLT_Mu17_Mu8_v4", "HLT_Mu17_Mu8_v5",
            "HLT_Mu17_Mu8_v6", "HLT_Mu17_Mu8_v7", "HLT_Mu17_Mu8_v8", "HLT_Mu17_Mu8_v9", "HLT_Mu17_Mu8_v10",
            "HLT_Mu17_Mu8_v11", "HLT_Mu17_Mu8_v12", "HLT_Mu17_Mu8_v13", "HLT_Mu17_Mu8_v14", "HLT_Mu17_Mu8_v15",
            "HLT_Mu17_Mu8_v16", "HLT_Mu17_Mu8_v17", "HLT_Mu17_Mu8_v18", "HLT_Mu17_Mu8_v19", "HLT_Mu17_Mu8_v20",
            "HLT_Mu17_Mu8_v21", "HLT_Mu17_Mu8_v22"]
    else:
        print "Run period", run, "is undefined. No json and jet corrections known."
        exit(1)

    cfg['Pipelines']['default']['Filter'].append ('json')
    cfg['Pipelines']['default']['Filter'].append ('hlt')
    cfg['GlobalProducer'] += ['hlt_selector']
    return cfg


def GetCuts(analysis='zjet'):
    """Return a set of default cuts for a given analysis."""
    cuts = {
        'zjet': {
            'Cuts': [
                'muon_eta',
                'muon_pt',
                'zpt',
                'zmass_window',
                'leadingjet_pt',
                'leadingjet_eta',
                'secondleading_to_zpt',
                'back_to_back',
            ],
            'CutMuonEta': 2.3,
            'CutMuonPt': 20.0,
            'CutZMassWindow': 20.0,
            'CutLeadingJetEta': 1.3,

            'CutSecondLeadingToZPt': 0.2,
            'CutBack2Back': 0.34,

            'CutZPt': 30.0,
            'CutLeadingJetPt': 12.0,

            'Filter': ['valid_z', 'valid_jet'],
        },
        'vbf': {
            'Cuts': [
                'leadingjet_pt',
                'secondjet_pt',
                'leadingjet_eta',
                'secondjet_eta',
                'rapidity_gap',
                'jet_mass',
                'muon_pt',
                'muon_eta',
                'zmass_window',
            ],
            'CutMuonEta': 2.3,
            'CutMuonPt': 20.0,
            'CutZMassWindow': 20.0,

            'CutLeadingJetPt': 30.0,
            'CutSecondJetPt': 30.0,
            'CutLeadingJetEta': 5.0,
            'CutSecondJetEta': 5.0,

            'CutRapidityGap': 4.0,
            'CutInvariantMass': 500.0,

            'Filter': ['valid_z', 'valid_jet'],
        },
    }
    if analysis not in cuts:
        print "There are no cuts defined for", analysis + "!"
        exit(1)
    return cuts[analysis]


def ApplyPUReweighting(conf, dataset, weightfile="data/pileup/puweights.json"):
    """Use pile-up reweighting.

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

    conf["EnablePuReweighting"] = True
    conf["XSection"] = d[dataset]["xsection"]
    conf["RecovertWeight"] = d[dataset]["weights"] + [0.0]*(100 - len(d[dataset]["weights"]))
    return conf


def Apply2ndJetReweighting(conf, dataset='powhegFall11', method='reco'):
    """Use 2nd jet reweighting.

    This function turns the 2nd jet reweighting on and sets the corresponding
    entries in the configuration. It is intended for powheg samples which
    need it. The weight factors are calculated via macros/weightCalc.py and
    stored in the following dictionary.
    """
    dataset += method
    d = {
        "powhegFall11reco" : [
            0, 0.96536, 0.97422, 0.975501, 0.973764, 0.96829, 0.969685, 0.969604, 0.972737, 0.976535, 0.990866, 1.02419, 1.04679, 1.08599, 1.1336, 1.1822, 1.26187, 1.34441, 1.4002, 1.52207, 1.59072, 1.67645, 1.74138, 1.80121, 1.88832, 2.08175, 2.18035, 2.25052, 2.24159, 2.34084, 2.3689, 2.45192, 2.78853, 2.84746, 2.61017, 2.94621, 2.99416, 3.05079, 3.11726, 3.19477, 3.28448, 3.38738, 3.50409, 3.63468, 3.77839, 3.93348, 4.09711, 4.26547, 4.43417, 4.59885, 4.75591, 4.90319, 5.04036, 5.16897, 5.29211, 5.41395, 5.53907, 5.67196, 5.81657, 5.97596, 6.1521, 6.34568, 6.55601, 6.78098, 7.01707, 7.25955, 7.50277, 7.74067, 7.96735, 8.17781, 8.36858, 8.53827, 8.68774, 8.82011, 8.94029, 9.05447, 9.16935, 9.2915, 9.42673, 9.57961, 9.75315, 9.9486, 10.1653, 10.4008, 10.6507, 10.9093, 11.1696, 11.4243, 11.6664, 11.8901, 12.0917, 12.2701, 12.4271, 12.5672, 12.6968, 12.8239, 12.9568, 13.1035, 13.2713, 13.4669, 13.696, 13.964, 14.2762, 14.6382, 15.0559, 15.5362, 16.0862, 16.7128, 17.4218, 18.216, 19.0937, 20.0461, 21.0556, 22.0957, 23.1321, 24.1272, 25.0463, 25.8648, 26.5738, 27.1835, 27.7231, 28.2382, 28.7879, 29.4432, 30.2872, 31.4189, 32.9606, 35.0708
        ],
        "powhegSummer12reco" : [],
    }

    if dataset not in d:
        print "No 2nd jet weights for this dataset:", dataset
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them in ClosureConfigBase or do not use Apply2ndJetReweighting."
        exit(0)

    conf["Enable2ndJetReweighting"] = 1
    conf["2ndJetWeight"] = d[dataset] + [1.0]*(300 - len(d[dataset]))
    return conf

def check_if_add(pipelinename, algo, forIncut = True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):
    #function that determines whether a consumer/variation is added to a pipeline
    check = ((forIncut and pipelinename == "default_"+algo)
        or (forAllevents and pipelinename == "default_" + algo + "nocuts")
        or (forIncutVariations and pipelinename is not "default_"+algo and "default_"+algo+"_" in pipelinename and "nocut" not in pipelinename)
        or (forAlleventsVariations and pipelinename is not "default_"+algo + "nocuts" and "default_"+algo + "nocuts" in pipelinename) )
    return check

def ExpandRange(pipelineDict, varName, vals,
                setRootFolder=True, includeSource=True,
                alsoForNoCuts=False, alsoForPtBins=True,
                correction="L1L2L3", onlyBasicQuantities=True):
    newDict = dict()

    for name, elem in pipelineDict.items():

        if (elem["Level"] == 1 )and ( ( not "nocuts" in name) or alsoForNoCuts ) and  ( correction in name) and (alsoForPtBins or "NoBinning_incut" in elem["RootFileFolder"]) and ('zcutsonly' not in name) and ('alleta' not in name):
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
                 alsoForPtBins=True,
                 onlyForNocuts=False,
                 onlyBasicQuantities = True):
    """Add pipelines with values between low and high for filtername

    This only works if the filter is lowercase and it uses two variables
    called Filter<FilterName>Low/High
    The foldername string can contain the variables {name}, {low} and {high}
    """
    newDict = {}
    for pipeline, subdict in pipelines.items():
        if (subdict["Level"] == 1
                and (not onlyOnIncut or "incut" in subdict["RootFileFolder"]) 
                and (alsoForPtBins or "NoBinning_incut" in subdict["RootFileFolder"])
                and (not onlyForNocuts or 'allevents' in subdict["RootFileFolder"]) ):
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
    """Add pipelines with values between low and high for filtername.

    This only works if the filter is lowercase and it uses two variables
    called Filter<FilterName>Low/High.
    The foldername string can contain the variables {name}, {low} and {high}.
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


def ExpandCutNoCut(pipelineDict, alletaFolder, zcutsFolder, isMC=False):
    newDict = dict()

    for name, elem in pipelineDict.items():

        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        algo = cutPipe["JetAlgorithm"]

        cutPipe["FilterInCutIgnored"] = 0
        cutPipe["Filter"].append ("incut")

        consumers = {
            'bin_balance_response': {
                'Name': "bin_response",
                'ProductName': "balresp_" + algo,
                'ResponseType': "bal",
            },
            'bin_balance_response': {
                'Name': "bin_response",
                'ProductName': "balresp_" + algo,
                'ResponseType': "bal",
            },
            'bin_balance_response_2ndJet': {
                'Name': "bin_response",
                'ProductName': "balrespjet2_" + algo,
                'ResponseType': "bal",
                'JetNumber': 2,
            },
            'bin_mpf_response': {
                'Name': "bin_response",
                'ProductName': "mpfresp_" + algo,
                'ResponseType': "mpf",
            },
            'bin_mpf-notypeI_response': {
                'Name': "bin_response",
                'ProductName': "mpfresp-raw_" +  algo,
                'ResponseType': "mpfraw",
            },
            'bin_twojet_response': {
                'Name': "bin_response",
                'ProductName': "baltwojet_" + algo,
                'ResponseType': "two",
            },
            'bin_zeppenfeld': {
                'Name': "bin_response",
                'ProductName': "zeppenfeld_" + algo,
                'ResponseType': "zep",
            },
        }

        consumers_mc = {
            'bin_muresp_response': {
                'Name': "bin_response",
                'ProductName': "muresp_" + algo,
                'ResponseType': "muresp",
            },
            'bin_z_response': {
                'Name': "bin_response",
                'ProductName': "zresp_" + algo,
                'ResponseType': "z",
            },
            'bin_recogen_response': {
                'Name': "bin_response",
                'ProductName': "recogen_" + algo,
                'ResponseType': "recogen",
            },
            'bin_recogen_response_2ndjet': {
                'Name': "bin_response",
                'ProductName': "recogenjet2_" + algo,
                'ResponseType': "recogen",
                'JetNumber': 2,
            },
        }

        consumers_gen = {
            'bin_genbal_response': {
                'Name': "bin_response",
                'ProductName': "genbal_" + algo,
                'ResponseType': "genbal",
            },
            'bin_genmpf_response': {
                'Name': "bin_response",
                'ProductName': "genmpf_" + algo,
                'ResponseType': "genmpf",
            },
            'bin_gentwo_response': {
                'Name': "bin_response",
                'ProductName': "gentwo_" + algo,
                'ResponseType': "gentwo",
            },
            'bin_genzep_response': {
                'Name': "bin_response",
                'ProductName': "genzep_" + algo,
                'ResponseType': "genzep",
            },
            'bin_parton_response': {
                'Name': "bin_response",
                'ProductName': "parton_" + algo,
                'ResponseType': "parton",
            },
            'bin_balparton_response': {
                'Name': "bin_response",
                'ProductName': "balparton_" + algo,
                'ResponseType': "balparton",
            },
            'bin_genbal_toparton_response': {
                'Name': "bin_response",
                'ProductName': "genbal-toparton_" + algo,
                'ResponseType': "genbal_toparton",
            },
            'bin_genbal_tobalparton_response': {
                'Name': "bin_response",
                'ProductName': "genbal-tobalparton_" + algo,
                'ResponseType': "genbal_tobalparton",
            },
            'bin_genquality_response': {
                'Name': "bin_response",
                'ProductName': "genquality_" + algo,
                'ResponseType': "quality",
            },
        }

        if isMC:
            cutPipe["Consumer"].update(consumers_mc)
            cutPipe["Consumer"].update(consumers_gen)
        cutPipe["Consumer"].update(consumers)
        # only add the nocut pipeline for the default (no binning)
        #if name == "default":
        newDict[name + "nocuts" ] = nocutPipe
        newDict[name] = cutPipe

        # a pipe without leadingjet eta cut
        if alletaFolder:
            alletaPipe = copy.deepcopy(cutPipe)
            alletaPipe["Cuts"].remove('leadingjet_eta')
            newDict[name + "alleta" ] = alletaPipe

        # a pipe with only muon and Z cuts
        if zcutsFolder:
            zcutsPipe = copy.deepcopy(cutPipe)
            zcutsPipe["Cuts"]=['muon_eta', 'muon_pt', 'zpt', 'zmass_window']
            newDict[name + "zcutsonly" ] = zcutsPipe

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
                  "XSource" : "npv",
                  "ProductName" : "cut-" + cut_name + "_npv_" + algoname })

    AddConsumer(pline, "cut_" + cut_name + "_zpt_" + algoname,
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "zpt",
                  "ProductName" : "cut-" + cut_name + "_zpt_" + algoname})

    AddConsumer(pline, "cut_" + cut_name + "_jet1eta_" + algoname,
                { "Name" : "generic_profile_consumer",
                  "RunUnfiltered" : 1,
                  "YSource" : "cutvalue",
                  "CutId" : cut_id,
                  "XSource" : "jet1eta",
                  "ProductName" : "cut-" + cut_name + "_jet1eta_" + algoname})

def AddCutConsumer( pipelineDict, algos):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if p == "default_" + algo:
                AddConsumer(pval, "cut_statistics",
                            { "Name": "cut_statistics" })
                AddConsumer(pval, "filter_statistics",
                            { "Name": "filter_statistics" })
                # for every intersting cut
                AddSingleCutConsumer(pval, "all", -1, algo )
                AddSingleCutConsumer(pval, "jet2toZpt", 16, algo )
                #AddSingleCutConsumer(pval, "backtoback", 32, algo )
                #AddSingleCutConsumer(pval, "zmass", 64, algo )
                #AddSingleCutConsumer(pval, "muonpt", 2, algo )
                #AddSingleCutConsumer(pval, "muoneta", 4, algo )
                #AddSingleCutConsumer(pval, "jeteta", 8, algo )

def AddLumiConsumer( pipelineDict, algos, forIncut = True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if check_if_add(p, algo, forIncut, forAllevents, forIncutVariations, forAlleventsVariations):

                def AddLumiConsumerEasy(y,x):
                    AddConsumerEasy(pval, { "Name" : "generic_profile_consumer", "YSource" : y, "XSource" : x, "ProductName" : "_".join([y,x,algo])})

                AddLumiConsumerEasy("eventcount", "intlumi")
                AddLumiConsumerEasy("ptbalance", "intlumi")

                for quantity in ["eventcount","zpt", "sumEt", "METpt", "jetsvalid",'jet1pt', 'ptbalance', 'mpf', 'npv',
                                'jet1neutralemfraction', 'jet1chargedemfraction', 'jet1neutralhadfraction', 
                                'jet1chargedhadfraction', 'jet1HFhadfraction', 'jet1HFemfraction', 'jet1photonfraction', 'jet1electronfraction']:
                    AddLumiConsumerEasy(quantity, "run")


def AddHltConsumer( pipelineDict, algoNames, hlt_names):
    for algo in algoNames:
        for hname in hlt_names:
            for p, pval in pipelineDict["Pipelines"].items():
                #print p
                if p == "default_" + algo + "nocuts":
                    AddConsumer(pval, "hlt_" + hname + "_prescale_run_" + algo,
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "run",
                                                  "ProductName" : "hlt-" + hname + "-prescale_run_" + algo})
                    AddConsumer(pval, "hlt_" + hname + "_prescale_lumi" + algo,
                                                { "Name" : "generic_profile_consumer",
                                                  "YSource" : "hltprescale",
                                                  "YSourceConfig" : hname,
                                                  "XSource" : "intlumi",
                                                  "ProductName" : "hlt-" + hname + "-prescale_lumi_" + algo})
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
                                              "ProductName" : "hltselectedprescale_lumi" + algo})
                AddConsumer(pval, "hlt_selected_prescale_run" + algo,
                                            { "Name" : "generic_profile_consumer",
                                              "YSource" : "selectedhltprescale",
                                              "YSourceConfig" : hname,
                                              "XSource" : "run",
                                              "ProductName" : "hltselectedprescale_run" + algo})


def ExpandPtBins( pipelineDict, ptbins, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        # dont do this for uncut events
        if ((not "nocuts" in name) and (not "alleta" in name) and (not "zcutsonly" in name)) :
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

    def AddCorrectionConsumer(pval, algo, level0, level1):
                name = level1.replace(level0,"")
                AddConsumer(pval, name+"_npv_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level1,
                              "Jet2Name" : algo + level0,
                              "XSource" : "npv",
                              "ProductName" : name+"_npv_" + algo})
                AddConsumer(pval, name+"_zpt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level1,
                              "Jet2Name" : algo + level0,
                              "XSource" : "zpt",
                              "ProductName" : name+"_zpt_" + algo})
                AddConsumer(pval, name+"_jet1pt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level1,
                              "Jet2Name" : algo + level0,
                              "XSource" : "jet1pt",
                              "ProductName" : name+"_jet1pt_" + algo})
                AddConsumer(pval, name+"_eta_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptratio", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level1,
                              "Jet2Name" : algo + level0,
                              "XSource" : "jet1eta",
                              "ProductName" : name+"_jet1eta_" + algo})

                AddConsumer(pval, name+"abs_npv_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptabsdiff", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level0,
                              "Jet2Name" : algo + level1,
                              "XSource" : "npv",
                              "ProductName" : name+"abs_npv_" + algo})
                AddConsumer(pval, name+"abs_jet1pt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptabsdiff", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level0,
                              "Jet2Name" : algo + level1,
                              "XSource" : "jet1pt",
                              "ProductName" : name+"abs_jet1pt_" + algo})
                AddConsumer(pval, name+"abs_zpt_" + algo,
                            { "Name" : "generic_profile_consumer",
                              "YSource" : "jetptabsdiff", "Jet1Num":0, "Jet2Num":0,
                              "Jet1Name" : algo + level0,
                              "Jet2Name" : algo + level1,
                              "XSource" : "zpt",
                              "ProductName" : name+"abs_zpt_" + algo})


    for algo in algoNames:
        for p, pval in conf["Pipelines"].items():
            if p == "default_" + algo:

                AddCorrectionConsumer(pval, algo, "", "L1")

                if level > 1:
                    AddCorrectionConsumer(pval, algo, "L1", "L1L2")

                if level > 2:
                    AddCorrectionConsumer(pval, algo, "", "L1L2L3")

                if l3residual:
                    AddCorrectionConsumer(pval, algo, "L1L2L3", "L1L2L3Res")

def AddQuantityPlots( pipelineDict, algos, forIncut = True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):

    def AddGenericProfileConsumer(x, y, jets=None):
        if jets is None:
            AddConsumerEasy(pval, { "Name" : "generic_profile_consumer", "YSource" : y, "XSource" : x, "ProductName" : "_".join([y,x,algo]) } )
        else:
            AddConsumerEasy(pval, { "Name" : "generic_profile_consumer", "YSource" : y, "XSource" : x, "ProductName" : "_".join([y,x,algo]),
                                 "Jet1Name":jets[0], "Jet2Name":jets[1], "Jet1Num":jets[2], "Jet2Num":jets[3]  } )

    def AddAbsDiff(x,y,obj1,obj2):
        AddConsumerEasy(pval, {"Name" : "generic_profile_consumer", "YSource" : y, "XSource" : x+"absdiff","XName1" : obj1,"XName2" : obj2,
                         "ProductName" : "_".join([y,"-".join(["delta"+x,obj1,obj2]),algo]) } )

    y_quantities = ['jet1pt', 'jet1abseta', 'jet2pt', 'jet2abseta', 'zpt', 'zabseta', 'npv', 'METpt-diff', 'METphi-diff', 'mpf-diff',
                         'ptbalance', 'mpf', 'mpf-raw', 'METpt', 'sumEt', 'METfraction', 'zphi', 'METphi', 'zmass']

    x_quantities = ['jet1pt', 'npv', 'jet1eta', 'jet1phi', 'jet2pt', 'jet2eta', 'jet2phi', 'zpt', 'zeta', 'zphi', 
                          'METpt', 'METphi', 'sumEt', 'jetsvalid', 'alpha']

    objects = ['z', 'jet1', 'jet2', 'MET']

    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if check_if_add(p, algo, forIncut, forAllevents, forIncutVariations, forAlleventsVariations):
                for x in x_quantities:
                    for y in y_quantities:
                        if x is not y:
                          AddGenericProfileConsumer(x,y)
                    AddGenericProfileConsumer(x, "jetptratio", jets=[algo,algo,0,1])
                    AddGenericProfileConsumer(x, "jetptabsdiff", jets=[algo,algo,0,1])

                for y in y_quantities:
                    AddGenericProfileConsumer("jetptratio", y, jets=[algo,algo,0,1])
                    AddGenericProfileConsumer("jetptabsdiff", y, jets=[algo,algo,0,1])
                    AddAbsDiff("eta", y, 'jet1', 'z')

                for obj1 in objects:
                    for obj2 in objects:
                        if obj1 is not obj2:
                            AddAbsDiff("eta", 'ptbalance', obj1, obj2)
                            for quantity  in ['ptbalance', 'mpf', 'mpf-raw', 'zpt', 'METpt', 'jet1pt', 'alpha']:
                                AddAbsDiff("phi", quantity, obj1, obj2)

def Add2DHistograms(pipelineDict, algos, forIncut = True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):
    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if check_if_add(p, algo, forIncut, forAllevents, forIncutVariations, forAlleventsVariations):
                    AddConsumerEasy(pval, {"Name" : "basic_twod_consumer", "ProductName": "2d"})
                    

def Add2DProfiles(pipelineDict, algos, forIncut = True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):

    def AddTwoDProfileConsumer(x, y, z):
            AddConsumerEasy(pval, { "Name" : "generic_profile2d_consumer",
                "XSource" : x,
                "YSource" : y,
                "ZSource" : z,
                "ProductName" : "2D_"+"_".join([z,x,y,algo]) } )

    def AddTwoDProfileEtaPhiConsumer(obj1, obj2, z):
            AddConsumerEasy(pval, { 
                    "Name" : "generic_profile2d_consumer", 
                    "XSource" : "phiabsdiff", "YName1":obj1, "YName2":obj2,
                    "YSource" : 'etaabsdiff', "XName1":obj1, "XName2":obj2,
                    "ZSource" : z,
                    "ProductName" : "2D_%s_deltaphi-%s-%s_deltaeta-%s-%s_" % (z, obj1, obj2, obj1, obj2) +algo   } )

    for algo in algos:
        for p, pval in pipelineDict["Pipelines"].items():
            if check_if_add(p, algo, forIncut, forAllevents, forIncutVariations, forAlleventsVariations):
                for z_quantity in ['jet1pt', 'ptbalance', 'mpf', 'jet1neutralemfraction', 'jet1chargedemfraction', 'jet1neutralhadfraction', 'jet1chargedhadfraction', 'jet1HFhadfraction', 'jet1HFemfraction', 'jet1photonfraction', 'jet1electronfraction']:
                    AddTwoDProfileConsumer('jet1phi', 'jet1eta', z_quantity)

                AddTwoDProfileConsumer('zphi', 'zeta', 'zpt')
                AddTwoDProfileConsumer('jet1phi', 'jet1eta', 'METpt')
                AddTwoDProfileConsumer('jet2phi', 'jet2eta', 'METpt')
                AddTwoDProfileConsumer('zpt', 'jet1eta', 'ptbalance')
                AddTwoDProfileConsumer('zpt', 'npv', 'ptbalance')
                AddTwoDProfileConsumer('jet2phi', 'jet2eta', 'jet2pt')
                AddTwoDProfileConsumer('jet2phi', 'jet2eta', 'ptbalance')
                AddTwoDProfileConsumer('jet2phi', 'jet2eta', 'mpf')

                AddTwoDProfileEtaPhiConsumer('z', 'jet1', 'jet1pt')
                AddTwoDProfileEtaPhiConsumer('jet1', 'jet2', 'jet1pt')
                AddTwoDProfileEtaPhiConsumer('z', 'jet1', 'ptbalance')
                AddTwoDProfileEtaPhiConsumer('jet1', 'jet2', 'ptbalance')

            #cuts
            if p == "default_" + algo:
                AddConsumerEasy(pval, {
                    'Name': "generic_profile2d_consumer",
                    'RunUnfiltered': 1,
                    'XSource': "phiabsdiff", 'XName1': "jet1", 'XName2': "jet2",
                    'YSource': "etaabsdiff", 'YName1': "jet1", 'YName2': "jet2",
                    'ZSource': "cutvalue", 'ZCutId': "-1",
                    'ProductName': "2D_cut-all_deltaphi-jet1-jet2_deltaeta-jet1-jet2_" + algo
                })

                AddConsumerEasy(pval, {
                    'Name': "generic_profile2d_consumer",
                    'RunUnfiltered': 1,
                    'XSource': "jet1phi",
                    'YSource': "jet1eta",
                    'ZSource': "cutvalue", 'ZCutId': "-1",
                    'ProductName': "2D_cut-all_jet1phi_jet1eta_" + algo
                })

                AddConsumerEasy(pval, {
                    'Name': "generic_profile2d_consumer",
                    'RunUnfiltered': 1,
                    'XSource': "jet2phi",
                    'YSource': "jet2eta",
                    'ZSource': "cutvalue", 'ZCutId': "-1",
                    'ProductName': "2D_cut-all_jet2phi_jet2eta_" + algo
                })

                AddConsumerEasy(pval, {
                    'Name': "generic_profile2d_consumer",
                    'RunUnfiltered': 1,
                    'XSource': "phiabsdiff", 'XName1': "jet1", 'XName2': "jet2",
                    'YSource': "etaabsdiff", 'YName1': "jet1", 'YName2': "jet2",
                    'ZSource': "cutvalue", "ZCutId": "16",
                    'ProductName': "2D_cut-jet2toZpt_deltaphi-jet1-jet2_deltaeta-jet1-jet2_" + algo
                })
    

def ReplaceWithQuantitiesBasic(pline):
    RemoveConsumer(pline, "quantities_all")
    AddConsumerNoConfig(pline, "quantities_basic")


def ExpandConfig(algoNames, conf_template, useFolders=True, FolderPrefix="",
        binning=GetDefaultBinning(), onlyBasicQuantities=False, expandptbins=True, alletaFolder=False, zcutsFolder=False):
    conf = copy.deepcopy(conf_template)

    # get globalalgorithms
    for algo in algoNames:
        if "AK5PF" not in conf["GlobalAlgorithms"] and "AK5PF" in algo:
            conf["GlobalAlgorithms"] += ["AK5PF", "AK5PFchs"]
        elif "AK7PF" not in conf["GlobalAlgorithms"] and "AK7PF" in algo:
            conf["GlobalAlgorithms"] += ["AK7PF", "AK7PFchs"]

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
            algoPipelines[p + "_" + algo] = pline

    conf["Pipelines"] = ExpandCutNoCut(algoPipelines, alletaFolder, zcutsFolder, conf['InputType'] == 'mc')

    # create pipelines for all bins
    if expandptbins:
        conf["Pipelines"] = ExpandPtBins(conf["Pipelines"], binning, True)

    #set the folder name
    for p, pval in conf["Pipelines"].items():
        ptVal = "NoBinning"

        if "ptbin" in pval["Filter"]:
            ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])
            if onlyBasicQuantities: ReplaceWithQuantitiesBasic ( pval )

        if "incut" in pval["Filter"]:
            if  pval["Cuts"] == ['muon_eta', 'muon_pt', 'zpt', 'zmass_window']:
                ptVal = ptVal + "_zcutsonly"
            elif 'leadingjet_eta' not in pval["Cuts"]:
                ptVal = ptVal + "_alleta"
            else:
                ptVal = ptVal + "_incut"

            if not ptVal == "NoBinning_incut":
                if onlyBasicQuantities: ReplaceWithQuantitiesBasic(pval)
        else:
            ptVal = ptVal + "_allevents"

        pval["RootFileFolder"] = FolderPrefix + ptVal

    return conf


def StoreSettings(settings, filename):
    f = open(filename, "w")

    jsonOut = str(settings)
    # make it json conform
    jsonOut = jsonOut.replace( "\'", "\"")

    try:
        import json
        # dont display config on console, it is annyoing
        #print json.dumps( settings, sort_keys=True, indent=4 )
        json.dump(settings, f, sort_keys=True, indent=4)
    except BaseException:
        f.write(jsonOut)
        print "No json Module found. Using fallback method ..."

    f.close()
    print "Configured", len(settings["Pipelines"]), "Pipelines"


def StoreGCDataset( settings, nickname, filename ):
    print "Generating " + filename

    # ordering is important in the .dbs file format
    cfile = open(filename, 'wb')
    cfile.write("[" +  nickname + "]\n")
    cfile.write("nickname = " + nickname + "\n")
    cfile.write("events = " + str(-len(settings['InputFiles'])) + "\n")

    path = os.path.split(settings['InputFiles'][0])[0]
    cfile.write("prefix = " + path + "\n")

    for f in settings['InputFiles']:
        cfile.write(os.path.split(f)[1] +  " = -1\n")

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
    config.set("jobs", "wall time", "1:50:00" )
    config.set("jobs", "monitor", "scripts" )

    config.add_section("local")
    config.set("local", "queue", "short")
    config.set("local", "delay output", "True")

    config.add_section("UserMod")
    # we can run more data files with one MC job as they don't contain that many
    # events
    if settings["InputType"] == "mc":
        config.set("UserMod", "files per job", 15)
    else:
        config.set("UserMod", "files per job", 25)
    config.set("UserMod", "executable", "gc-run-closure.sh")
    config.set("UserMod", "subst files", "gc-run-closure.sh")
    config.set("UserMod", "input files", GetBasePath() + "external/lib/libboost_regex.so.1.45.0")

    config.add_section("storage")
    config.set("storage", "se path", "dir://" + output_folder)
    config.set("storage", "se output files", settings["OutputPath"] + ".root")
    config.set("storage", "se output pattern", "@NICK@_job_@MY_JOBID@.root")

    # Writing our configuration file to 'example.cfg'
    cfile = open(filename, 'wb')
    config.write(cfile)
    cfile.close()


def StoreMergeScript ( settings, nickname, filename, output_folder, merge_folder='temp'):
    print "Generating", filename
    cfile = open(filename, 'wb')
    cfile.write("hadd " + output_folder + settings["OutputPath"] + ".root " + output_folder + nickname + "_job_*.root\n" )
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)
    print "Generating", filename.replace("merge", "parallelmerge")
    mfile = open(filename.replace("merge","parallelmerge"), 'wb')
    #mfile.write("mkdir -p " + output_folder.replace("out", merge_folder) + "\n")
    mfile.write(GetBasePath()+"scripts/parallelmerge.sh " + output_folder + " 10")
    os.chmod(filename.replace("merge","parallelmerge"), stat.S_IRWXU)

def StoreShellRunner(settings, nickname, filename):
    print "Generating " + filename
    cfile = open(filename, 'wb')
    cfile.write("echo $FILE_NAMES\n")
    cfile.write("cd " + GetCMSSWPath() +"\n")
    if "CMSSW_5_" in GetCMSSWPath():
        cfile.write("export VO_CMS_SW_DIR=/wlcg/sw/cms\n")
        cfile.write("export SCRAM_ARCH=slc5_amd64_gcc462\n")
        cfile.write("source $VO_CMS_SW_DIR/cmsset_default.sh\n")
    else:
        cfile.write("source /wlcg/sw/cms/experimental/cmsset_default.sh\n")
    cfile.write("eval `scram runtime -sh`\n")
    cfile.write("cd -\n")
    cfile.write("cd "+GetBasePath()+"\n")
    cfile.write("source "+ GetBasePath() + "scripts/ClosureEnv.sh\n")
    cfile.write("cd -\n")
    cfile.write( GetBasePath() + "closure " + GetBasePath() + "cfg/closure/" + nickname + ".py.json" )
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)


def Run(settings, arguments):
    """Run this config with closure

    The options are:
      --storeonly Just generate the json config file and exit
      --fast      Run over a few files to see if it works
      --batch     Split into jobs for processing on a cluster
    """
    filename = arguments[0] + ".json"
    StoreSettings(settings, filename)

    base_path = GetBasePath()
    work_path = GetWorkPath()

    if len(arguments) > 1 and "--storeonly" in arguments:
        print "The settings were stored to", filename
        exit(0)

    if len(arguments) <= 1 or "batch" not in arguments[1]:  # both 'batch' and '--batch' usable
        subprocess.call(["./closure",filename])
    else:
        nickname = os.path.split(filename)[1]
        nickname = nickname.split(".")[0]
        print "Generating GC configs with nickname " + nickname + " ..."
        # store the input files in gc format
        if not os.path.exists( work_path + "work/" ) :
            os.mkdir( work_path + "work/" )
        if not os.path.exists( work_path + "work/" + nickname ) :
            os.mkdir( work_path + "work/" + nickname )
        if not os.path.exists( work_path + "work/" + nickname + "/out/" ) :
            os.mkdir( work_path + "work/" + nickname + "/out/" )

        StoreGCDataset(settings, nickname, work_path + "work/" + nickname + "/" + nickname + ".dbs")
        StoreGCConfig(settings, nickname, work_path + "work/" + nickname + "/" + nickname + ".conf")
        StoreGCCommon(settings, nickname, work_path + "work/" + nickname + "/gc_common.conf", work_path + "work/" + nickname + "/out/")
        StoreMergeScript(settings, nickname, work_path + "work/" + nickname + "/merge.sh", work_path + "work/" + nickname + "/out/")
        StoreShellRunner(settings, nickname, work_path + "work/" + nickname + "/gc-run-closure.sh")

        # generate merge script
        print "The config files are prepared in", work_path + "work/" + nickname
        print "Go there and start grid-control with", nickname + ".conf!"
    try:
        import pynotify
        if pynotify.init("CalibFW resp_cuts"):
            n = pynotify.Notification("CalibFW resp_cuts", "run with config " + filename + " done")
            n.show()
    except:
        pass

