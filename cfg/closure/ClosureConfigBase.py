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

    if args is not None and len(args) > 1 and "--fast" == args[1]:
        try:
            if len(args) > 3:
                inputfiles = inputfiles[int(args[2]):int(args[3])]
            else:
                inputfiles = inputfiles[-int(args[2]):]
        except:
            inputfiles = inputfiles[-3:]
    return inputfiles


def FindFileInList(conf, number):
    for f in conf['InputFiles']:
        if "_" + str(number) in f:
            print conf['InputFiles'].index(f)


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
    jsons = glob.glob('data/json/Cert*' + variant + '*.txt')
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
        'SkipEvents': 0,
        'EventCount': -1,
        'GlobalAlgorithms': [],
        'GlobalProducer': [    # The order of these producers is important!
            'valid_muon_producer', 'z_producer', 'valid_jet_producer',
            'corr_jet_producer', 'typeImet_producer', 'jet_sorter_producer',
            'unclustered_energy_producer',
        ],
        'L1Correction': 'L1FastJet',
        'EnableMetPhiCorrection': False,
        'MetPhiCorrectionParameters': [],
        'EnablePuReweighting': False,
        'Enable2ndJetReweighting': False,
        'EnableSampleReweighting': False,
        'EnableLumiReweighting': False,
        'HcalCorrection': 0.0,
        'Jec': "default",
        'JsonFile': "default",
        'InputFiles': [],
        'OutputPath': "closure_" + inputtype + "_" + run,
        'MuonID2011': (run == '2011'),
        'Pipelines': {
            'default': {
                'Level': 1,
                'JetAlgorithm': "default",
                'RootFileFolder': "",
                'Consumer': {},
            }
        },
        'InputType': inputtype,
        'Tagged': True,
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
        ApplyPUReweighting(cfg, 'mc11_160404-180252_7TeV_ReRecoNov08_v2')
    elif run == '2012':
        cfg['Jec'] = GetBasePath() + "data/jec_data/Summer13_V5_MC"
        cfg['MetPhiCorrectionParameters'] = [0.1166, 0.0200, 0.2764, -0.1280]
        ApplyPUReweighting(cfg, 'kappa539_MC12_madgraph_190456-208686_8TeV_22Jan2013ReReco')
    else:
        print "MC period", run, "is undefined. No jet corrections known."
        exit(0)

    cfg['GlobalProducer'] += ['jet_matcher', 'gen_producer', 'weight_producer']
    cfg['EnableLumiReweighting'] = True
    cfg['XSection'] = -1.0
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
            "HLT_Mu13_Mu8_v15", "HLT_Mu13_Mu8_v16", "HLT_Mu13_Mu8_v17", "HLT_Mu13_Mu8_v18",
            # Mu17_Mu8 Trigger
            "HLT_Mu17_Mu8_v10", "HLT_Mu17_Mu8_v11"

            ]
    elif run == '2012':
        cfg['Jec'] = GetBasePath() + "data/jec_data/Summer13_V5_DATA"
        cfg['MetPhiCorrectionParameters'] = [0.2661, 0.3217, -0.2251, -0.1747]
        cfg['JsonFile'] = GetBasePath() + "data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
        cfg['HltPaths'] = [
            "HLT_Mu17_Mu8_v1", "HLT_Mu17_Mu8_v2", "HLT_Mu17_Mu8_v3", "HLT_Mu17_Mu8_v4", "HLT_Mu17_Mu8_v5",
            "HLT_Mu17_Mu8_v6", "HLT_Mu17_Mu8_v7", "HLT_Mu17_Mu8_v8", "HLT_Mu17_Mu8_v9", "HLT_Mu17_Mu8_v10",
            "HLT_Mu17_Mu8_v11", "HLT_Mu17_Mu8_v12", "HLT_Mu17_Mu8_v13", "HLT_Mu17_Mu8_v14", "HLT_Mu17_Mu8_v15",
            "HLT_Mu17_Mu8_v16", "HLT_Mu17_Mu8_v17", "HLT_Mu17_Mu8_v18", "HLT_Mu17_Mu8_v19", "HLT_Mu17_Mu8_v20",
            "HLT_Mu17_Mu8_v21", "HLT_Mu17_Mu8_v22"]
    else:
        print "Run period", run, "is undefined. No json and jet corrections known."
        exit(1)

    cfg['Pipelines']['default']['Filter'].append('json')
    cfg['Pipelines']['default']['Filter'].append('hlt')
    cfg['GlobalProducer'] += ['hlt_selector']
    return cfg


def GetCuts(analysis='zjet'):
    """Return a set of default cuts for a given analysis."""
    cuts = {
        'zjet': {
            'GenCuts': False,
            'Cuts': [
                'secondleading_to_zpt',
                'zpt',
                'leadingjet_pt',
                'leadingjet_eta',
                'muon_eta',
                'muon_pt',
                'zmass_window',
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

            'Filter': ['valid_z', 'valid_jet', 'metfilter'],
        },
        'vbf': {
            'GenCuts': False,
            'Cuts': [
                'muon_pt',
                'muon_eta',
                'zmass_window',
                'zpt',
                'leadingjet_pt',
                'secondjet_pt',
                'leadingjet_eta',
                'secondjet_eta',
                'rapidity_gap',
                'jet_mass',
            ],
            'CutMuonEta': 2.3,
            'CutMuonPt': 20.0,
            'CutZMassWindow': 20.0,
            'CutZPt': 10,

            'CutLeadingJetPt': 30.0,
            'CutSecondJetPt': 30.0,
            'CutLeadingJetEta': 5.0,
            'CutSecondJetEta': 5.0,

            'CutRapidityGap': 4.0,
            'CutInvariantMass': 500.0,

            'Filter': ['valid_z', 'valid_jet', 'metfilter'],
        },
    }
    if analysis not in cuts:
        print "There are no cuts defined for", analysis + "!"
        exit(1)
    return cuts[analysis]


def treeconfig(conf, quantities=None):
    for p, pval in conf["Pipelines"].items():
        if quantities == None:
            pval['QuantitiesVector'] = ["zpt", "zeta", "zy",
             "zphi", "zmass", "npv", "rho",
                 "run", "weight", "jet1pt", "jet1eta", "jet1phi", "mpf", "rawmpf",
                 "METpt", "METphi", "rawMETpt", "rawMETphi", "sumEt", "jet1photonfraction",
                 "jet1chargedemfraction", "jet1chargedhadfraction", "jet1neutralhadfraction",
                 "jet1muonfraction", "jet1HFhadfraction", "jet1HFemfraction",
                 "jet2pt", "jet2eta", "jet2phi", "uept", "uephi", "ueeta",
                 "otherjetspt", "otherjetseta", "otherjetsphi",
                 "mupluspt", "mupluseta", "muplusphi",
                 "muminuspt", "muminuseta", "muminusphi"
                  ]
            if conf['Tagged'] == True:
                pval['QuantitiesVector'] += [
                    "qglikelihood", "qgmlp", "trackcountinghigheffbjettag",
                    "trackcountinghighpurbjettag", "jetprobabilitybjettag",
                    "jetbprobabilitybjettag", "softelectronbjettag",
                    "softmuonbjettag", "softmuonbyip3dbjettag",
                    "softmuonbyptbjettag", "simplesecondaryvertexbjettag",
                    "combinedsecondaryvertexbjettag", "combinedsecondaryvertexmvabjettag",
                    "jet1puJetFull", "jet1puJetIDFull", "jet1puJetIDFullLoose", "jet1puJetIDFullMedium", "jet1puJetIDFullTight",
                    "jet1puJetCutBased", "jet1puJetIDCutBased", "jet1puJetIDCutBasedLoose", "jet1puJetIDCutBasedMedium", "jet1puJetIDCutBasedTight",
                    "jet2puJetFull", "jet2puJetIDFull", "jet2puJetIDFullLoose", "jet2puJetIDFullMedium", "jet2puJetIDFullTight",
                    "jet2puJetCutBased", "jet2puJetIDCutBased", "jet2puJetIDCutBasedLoose", "jet2puJetIDCutBasedMedium", "jet2puJetIDCutBasedTight",
                ]
            if conf['InputType'] == 'mc':
                pval['QuantitiesVector'] += [
                    "genjet1pt", "genjet1eta", "genjet1phi", "genjet2pt",
                    "matchedgenjet1pt", "genmpf",
                    "algoflavour", "physflavour",
                            ]
        else:
            pval['QuantitiesVector'] = quantities

        pval['QuantitiesString'] = ":".join(pval['QuantitiesVector'])

        # replace the quantites_vector with integers according to the dictionary
        #new_quantities = []
        #for q in pval['QuantitiesVector']:
        #    new_quantities += [closure_dict[q]]
        #pval['QuantitiesVector'] = new_quantities

        pval['Cuts'].remove("leadingjet_eta")
        pval['Cuts'].remove("secondleading_to_zpt")

        pval['Treename'] = pval['RootFileFolder'][10:]
        pval['RootFileFolder'] = ""
        RemoveConsumer(pval, "quantities_all")

        AddConsumerNoConfig(pval, "tree")
    return conf


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
    conf["RecovertWeight"] = d[dataset]["weights"] + [0.0] * (100 - len(d[dataset]["weights"]))
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
        "powhegFall11reco": [
            0, 0.96536, 0.97422, 0.975501, 0.973764, 0.96829, 0.969685, 0.969604, 0.972737, 0.976535, 0.990866, 1.02419, 1.04679, 1.08599, 1.1336, 1.1822, 1.26187, 1.34441, 1.4002, 1.52207, 1.59072, 1.67645, 1.74138, 1.80121, 1.88832, 2.08175, 2.18035, 2.25052, 2.24159, 2.34084, 2.3689, 2.45192, 2.78853, 2.84746, 2.61017, 2.94621, 2.99416, 3.05079, 3.11726, 3.19477, 3.28448, 3.38738, 3.50409, 3.63468, 3.77839, 3.93348, 4.09711, 4.26547, 4.43417, 4.59885, 4.75591, 4.90319, 5.04036, 5.16897, 5.29211, 5.41395, 5.53907, 5.67196, 5.81657, 5.97596, 6.1521, 6.34568, 6.55601, 6.78098, 7.01707, 7.25955, 7.50277, 7.74067, 7.96735, 8.17781, 8.36858, 8.53827, 8.68774, 8.82011, 8.94029, 9.05447, 9.16935, 9.2915, 9.42673, 9.57961, 9.75315, 9.9486, 10.1653, 10.4008, 10.6507, 10.9093, 11.1696, 11.4243, 11.6664, 11.8901, 12.0917, 12.2701, 12.4271, 12.5672, 12.6968, 12.8239, 12.9568, 13.1035, 13.2713, 13.4669, 13.696, 13.964, 14.2762, 14.6382, 15.0559, 15.5362, 16.0862, 16.7128, 17.4218, 18.216, 19.0937, 20.0461, 21.0556, 22.0957, 23.1321, 24.1272, 25.0463, 25.8648, 26.5738, 27.1835, 27.7231, 28.2382, 28.7879, 29.4432, 30.2872, 31.4189, 32.9606, 35.0708
        ],
        "powhegSummer12reco": [],
    }

    if dataset not in d:
        print "No 2nd jet weights for this dataset:", dataset
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them in ClosureConfigBase or do not use Apply2ndJetReweighting."
        exit(0)

    conf["Enable2ndJetReweighting"] = True
    conf["2ndJetWeight"] = d[dataset] + [1.0] * (300 - len(d[dataset]))
    return conf


def ApplySampleReweighting(conf, sample="herwig", referencelumi_fbinv=1.0):
    """Weights for pt hat binned samples"""
    picobarn2femtobarn = 1000
    d = {
        "herwig": [
            0.0,  # 400.8834/6167020,# 0-15 (not existing, prep/das inconsistent)
            70.55123   /  200000,  #  15-20
            77.53533   /  150154,  #  20-30 (old sample)
            62.74567   /  150000,  #  30-50
            28.73806   /  100160,  #  50-80 (old sample)
            9.745931   /   96000,  #  80-120
            2.810025   /   98560,  # 120-170
            0.7702934  /  100000,  # 170-230
            0.214268   /   96640,  # 230-300
            0.08858213 /   90517,  # 300-inf
        ],
    }

    if sample not in d:
        print "No sample weights for this dataset:", sample
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them in ClosureConfigBase or do not use ApplySampleReweighting."
        exit(0)

    result = [picobarn2femtobarn * referencelumi_fbinv * w for w in d[sample]]
    conf["EnableSampleReweighting"] = True
    conf["SampleWeight"] = result
    return conf


def check_if_add(pipelinename, algo, forIncut=True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):
    #function that determines whether a consumer/variation is added to a pipeline
    check = ((forIncut and (pipelinename == "default_" + algo or pipelinename == "default_" + algo + "zcutsonly" or pipelinename == "default_" + algo + "alleta"))
        or (forAllevents and pipelinename == "default_" + algo + "nocuts")
        or (forIncutVariations and pipelinename is not "default_" + algo and "default_" + algo + "_" in pipelinename and "nocut" not in pipelinename)
        or (forAlleventsVariations and pipelinename is not "default_" + algo + "nocuts" and "default_" + algo + "nocuts" in pipelinename))
    return check


def ExpandRange(pipelineDict, varName, vals,
                setRootFolder=True, includeSource=True,
                alsoForNoCuts=False, alsoForPtBins=True,
                correction="L1L2L3", onlyBasicQuantities=True):
    newDict = {}

    for name, elem in pipelineDict.items():

        if ((elem["Level"] == 1)
                and ((not "nocuts" in name) or alsoForNoCuts)
                and (correction in name)
                and (alsoForPtBins or "NoBinning_incut" in elem["RootFileFolder"])
                and ('zcutsonly' not in name)
                and ('alleta' not in name)):
            for v in vals:
                newPipe = copy.deepcopy(elem)
                newPipe[varName] = v

                #only do basic plots
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic(newPipe)

                varadd = "_var_" + varName + "_" + str(v).replace(".", "_")
                newName = name + varadd
                newRootFileFolder = newPipe["RootFileFolder"] + varadd
                newDict[newName] = newPipe
                if setRootFolder:
                    newDict[newName]["RootFileFolder"] = newRootFileFolder

    if includeSource:
        return dict(pipelineDict.items() + newDict.items())
    else:
        return newDict


def ExpandRange2(pipelines, filtername, low, high=None,
                 foldername="var_{name}_{low}to{high}",
                 includeSource=False, onlyOnIncut=True,
                 alsoForPtBins=True,
                 onlyForNocuts=False,
                 onlyBasicQuantities=True,
                 forAlleta=False):
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
                and (not onlyForNocuts or 'allevents' in subdict["RootFileFolder"])
                or (forAlleta and 'alleta' in subdict["RootFileFolder"])
                ):

            for l, h in zip(low, high or low):
                # copy existing pipeline (subdict) and modify it
                newpipe = copy.deepcopy(subdict)

                #only do basic plots
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic(newpipe)
                if filtername == "JetEta":
                    newpipe["CutLeadingJetEta"] = 5.0

                #print(new_pipe)
                newpipe["Filter"].append(filtername.lower())
                if high is None:
                    foldername = foldername.replace("to{high}", "")
                    newpipe["Filter" + filtername] = l
                    if filtername == "Flavour":
                        l = {1: "u", 2: "d", 3: "s", 4: "c", 5: "b", 6: "t", 21: "g", 123: "uds", 123456: "q"}[l]
                else:
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
        return dict(pipelines.items() + newDict.items())
    else:
        return newDict


def ExpandRange2Cut(pipelines, cutname, low, high=None,
                foldername="var_{name}_{low}to{high}",
                includeSource=True, onlyOnIncut=True,
                onlyBasicQuantities=True):
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
                    ReplaceWithQuantitiesBasic(newpipe)

                #print(new_pipe)
                newpipe["Cut" + cutname.replace("_", "") + "Low"] = l
                newpipe["Cut" + cutname.replace("_", "") + "High"] = h
                f = foldername.format(name=cutname, low=l, high=h)
                f = "_" + f.replace(".", "_")

                newName = pipeline + f
                newRootFileFolder = newpipe["RootFileFolder"] + f
                newDict[newName] = newpipe
                if foldername is not None:
                    newDict[newName]["RootFileFolder"] = newRootFileFolder
    if includeSource:
        return dict(pipelines.items() + newDict.items())
    else:
        return newDict


def AddConsumer(pline, name, config):
    pline["Consumer"][name] = config


def AddMetaDataProducer(pline, name, config):
    pline["MetaDataProducer"][name] = config


def AddConsumerEasy(pline, consumer):
    pline["Consumer"][consumer["ProductName"]] = consumer


def AddConsumerNoConfig(pline, consumer_name):
    cons_dict = {"Name": consumer_name}
    pline["Consumer"][consumer_name] = cons_dict


def RemoveConsumer(pline, consumer_name):
    if consumer_name in pline["Consumer"]:
        del pline["Consumer"][consumer_name]


def AddMetaDataProducerEasy(pline, producer_name):
    pline["MetaDataProducer"][consumer["Name"]] = producer_name


def ExpandCutNoCut(pipelineDict, alletaFolder, zcutsFolder, isMC=False,
                   addResponse=True, nocutFolder=True):
    newDict = dict()

    for name, elem in pipelineDict.items():

        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        algo = cutPipe["JetAlgorithm"]

        cutPipe["FilterInCutIgnored"] = 0
        cutPipe["Filter"].append("incut")

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
                'ProductName': "mpfresp-raw_" + algo,
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
            # flavour responses, JetNumber is misused as flavour
           'bin_gluonbal_response': {
                'Name': "bin_response",
                'ProductName': "balgluon_" + algo,
                'ResponseType': "flavourbal",
                'JetNumber': 21,
            },
           'bin_udsbal_response': {
                'Name': "bin_response",
                'ProductName': "baluds_" + algo,
                'ResponseType': "flavourbal",
                'JetNumber': 123,
            },
           'bin_cbal_response': {
                'Name': "bin_response",
                'ProductName': "balc_" + algo,
                'ResponseType': "flavourbal",
                'JetNumber': 4,
            },
           'bin_bbal_response': {
                'Name': "bin_response",
                'ProductName': "balb_" + algo,
                'ResponseType': "flavourbal",
                'JetNumber': 5,
            },
           'bin_gluonmpf_response': {
                'Name': "bin_response",
                'ProductName': "mpfgluon_" + algo,
                'ResponseType': "flavourmpf",
                'JetNumber': 21,
            },
           'bin_udsmpf_response': {
                'Name': "bin_response",
                'ProductName': "mpfuds_" + algo,
                'ResponseType': "flavourmpf",
                'JetNumber': 123,
            },
           'bin_cmpf_response': {
                'Name': "bin_response",
                'ProductName': "mpfc_" + algo,
                'ResponseType': "flavourmpf",
                'JetNumber': 4,
            },
           'bin_bmpf_response': {
                'Name': "bin_response",
                'ProductName': "mpfb_" + algo,
                'ResponseType': "flavourmpf",
                'JetNumber': 5,
            },
           'bin_gluonrecogen_response': {
                'Name': "bin_response",
                'ProductName': "recogengluon_" + algo,
                'ResponseType': "flavourrecogen",
                'JetNumber': 21,
            },
           'bin_udsrecogen_response': {
                'Name': "bin_response",
                'ProductName': "recogenuds_" + algo,
                'ResponseType': "flavourrecogen",
                'JetNumber': 123,
            },
           'bin_crecogen_response': {
                'Name': "bin_response",
                'ProductName': "recogenc_" + algo,
                'ResponseType': "flavourrecogen",
                'JetNumber': 4,
            },
           'bin_brecogen_response': {
                'Name': "bin_response",
                'ProductName': "recogenb_" + algo,
                'ResponseType': "flavourrecogen",
                'JetNumber': 5,
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
            # gen flavour responses
           'bin_gluongen_response': {
                'Name': "bin_response",
                'ProductName': "gengluon_" + algo,
                'ResponseType': "flavourgenbal",
                'JetNumber': 21,
            },
           'bin_udsgen_response': {
                'Name': "bin_response",
                'ProductName': "genuds_" + algo,
                'ResponseType': "flavourgenbal",
                'JetNumber': 123,
            },
           'bin_cgen_response': {
                'Name': "bin_response",
                'ProductName': "genc_" + algo,
                'ResponseType': "flavourgenbal",
                'JetNumber': 4,
            },
           'bin_bgen_response': {
                'Name': "bin_response",
                'ProductName': "genb_" + algo,
                'ResponseType': "flavourgenbal",
                'JetNumber': 5,
            },
        }

        if addResponse:
            if isMC:
                cutPipe["Consumer"].update(consumers_mc)
                cutPipe["Consumer"].update(consumers_gen)
            cutPipe["Consumer"].update(consumers)
        # only add the nocut pipeline for the default (no binning)
        #if name == "default":
        if nocutFolder:
            newDict[name + "nocuts"] = nocutPipe
        newDict[name] = cutPipe

        # a pipe without leadingjet eta cut
        if alletaFolder:
            alletaPipe = copy.deepcopy(cutPipe)
            alletaPipe["Cuts"].remove('leadingjet_eta')
            newDict[name + "alleta"] = alletaPipe

        # a pipe with only muon and Z cuts
        if zcutsFolder:
            zcutsPipe = copy.deepcopy(cutPipe)
            zcutsPipe["Cuts"] = ['muon_eta', 'muon_pt', 'zpt', 'zmass_window']
            newDict[name + "zcutsonly"] = zcutsPipe

    return newDict


def Expand(pipelineDict, expandCount, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        for i in range(expandCount):
            newPipe = copy.deepcopy(elem)
            newDict[name + str(i)] = newPipe

    if includeSource:
        return dict(pipelineDict.items() + newDict.items())
    else:
        return newDict


def AddHltConsumer(pipelineDict, algoNames, hlt_names):
    for algo in algoNames:
        for hname in hlt_names:
            for p, pval in pipelineDict["Pipelines"].items():
                #print p
                if p == "default_" + algo + "nocuts":
                    AddConsumer(pval, "hlt_" + hname + "_prescale_run_" + algo,
                                                {"Name": "generic_profile_consumer",
                                                  "YSource": "hltprescale",
                                                  "YSourceConfig": hname,
                                                  "XSource": "run",
                                                  "ProductName": "hlt-" + hname + "-prescale_run_" + algo})
                    AddConsumer(pval, "hlt_" + hname + "_prescale_lumi" + algo,
                                                {"Name": "generic_profile_consumer",
                                                  "YSource": "hltprescale",
                                                  "YSourceConfig": hname,
                                                  "XSource": "intlumi",
                                                  "ProductName": "hlt-" + hname + "-prescale_lumi_" + algo})
    # plot the selcted hlt
    for algo in algoNames:
        for p, pval in pipelineDict["Pipelines"].items():
            #print p
            if p == "default_" + algo + "nocuts":
                AddConsumer(pval, "hlt_selected_prescale_lumi" + algo,
                                            {"Name": "generic_profile_consumer",
                                              "YSource": "selectedhltprescale",
                                              "YSourceConfig": hname,
                                              "XSource": "intlumi",
                                              "ProductName": "hltselectedprescale_lumi" + algo})
                AddConsumer(pval, "hlt_selected_prescale_run" + algo,
                                            {"Name": "generic_profile_consumer",
                                              "YSource": "selectedhltprescale",
                                              "YSourceConfig": hname,
                                              "XSource": "run",
                                              "ProductName": "hltselectedprescale_run" + algo})


def ExpandPtBins(pipelineDict, ptbins, includeSource):
    newDict = dict()

    for name, elem in pipelineDict.items():
        # dont do this for uncut events
        if ((not "nocuts" in name) and (not "alleta" in name) and (not "zcutsonly" in name)):
            i = 0
            for upper in ptbins[1:]:
                ptbinsname = "Bin" + str(ptbins[i]) + "To" + str(upper)

                newPipe = copy.deepcopy(elem)

                newPipe["Filter"].append("ptbin")

                newPipe["FilterPtBinLow"] = ptbins[i]
                newPipe["FilterPtBinHigh"] = upper

                newDict[name + "_" + ptbinsname] = newPipe
                i = i + 1

    if includeSource:
        return dict(pipelineDict.items() + newDict.items())
    else:
        return newDict


def AddCorrectionPlots(conf, algoNames, l3residual=False, level=3, forIncut=True, forAllevents=False, forIncutVariations=False, forAlleventsVariations=False):

    def AddCorrectionConsumer(pval, algo, level0, level1):
                name = level1.replace(level0, "")
                AddConsumer(pval, name + "_npv_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptratio", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level1,
                              "Jet2Name": algo + level0,
                              "XSource": "npv",
                              "ProductName": name + "_npv_" + algo})
                AddConsumer(pval, name + "_zpt_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptratio", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level1,
                              "Jet2Name": algo + level0,
                              "XSource": "zpt",
                              "ProductName": name + "_zpt_" + algo})
                AddConsumer(pval, name + "_jet1pt_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptratio", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level1,
                              "Jet2Name": algo + level0,
                              "XSource": "jet1pt",
                              "ProductName": name + "_jet1pt_" + algo})
                AddConsumer(pval, name + "_eta_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptratio", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level1,
                              "Jet2Name": algo + level0,
                              "XSource": "jet1eta",
                              "ProductName": name + "_jet1eta_" + algo})

                AddConsumer(pval, name + "abs_npv_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptabsdiff", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level0,
                              "Jet2Name": algo + level1,
                              "XSource": "npv",
                              "ProductName": name + "abs_npv_" + algo})
                AddConsumer(pval, name + "abs_jet1pt_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptabsdiff", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level0,
                              "Jet2Name": algo + level1,
                              "XSource": "jet1pt",
                              "ProductName": name + "abs_jet1pt_" + algo})
                AddConsumer(pval, name + "abs_zpt_" + algo,
                            {"Name": "generic_profile_consumer",
                              "YSource": "jetptabsdiff", "Jet1Num": 0, "Jet2Num": 0,
                              "Jet1Name": algo + level0,
                              "Jet2Name": algo + level1,
                              "XSource": "zpt",
                              "ProductName": name + "abs_zpt_" + algo})

    for algo in algoNames:
        for p, pval in conf["Pipelines"].items():
            if check_if_add(p, algo, forIncut, forAllevents, forIncutVariations, forAlleventsVariations):

                AddCorrectionConsumer(pval, algo, "", "L1")

                if level > 1:
                    AddCorrectionConsumer(pval, algo, "L1", "L1L2")

                if level > 2:
                    AddCorrectionConsumer(pval, algo, "", "L1L2L3")

                if l3residual:
                    AddCorrectionConsumer(pval, algo, "L1L2L3", "L1L2L3Res")


def ReplaceWithQuantitiesBasic(pline):
    RemoveConsumer(pline, "quantities_all")
    AddConsumerNoConfig(pline, "quantities_basic")


def ExpandConfig(algoNames, conf_template, useFolders=True, FolderPrefix="",
        binning=GetDefaultBinning(), onlyBasicQuantities=False, expandptbins=True,
        alletaFolder=False, zcutsFolder=False, addResponse=True, nocutsFolder=True):
    conf = copy.deepcopy(conf_template)

    # get globalalgorithms
    for algo in algoNames:
        if "AK5PF" not in conf["GlobalAlgorithms"] and "AK5PF" in algo:
            conf["GlobalAlgorithms"] += ["AK5PF", "AK5PFchs"]
        elif "AK7PF" not in conf["GlobalAlgorithms"] and "AK7PF" in algo:
            conf["GlobalAlgorithms"] += ["AK7PF", "AK7PFchs"]

    # generate folder names
    srcFolder = []
    for i in range(len(binning) - 1):
        srcFolder += ["Pt" + str(binning[i]) + "to" + str(binning[i + 1]) + "_incut"]

    algoPipelines = {}

    # generate pipelines for all algorithms
    for algo in algoNames:
        for p, pval in conf["Pipelines"].items():
            pline = copy.deepcopy(pval)
            pline["JetAlgorithm"] = algo
            algoPipelines[p + "_" + algo] = pline

    conf["Pipelines"] = ExpandCutNoCut(algoPipelines, alletaFolder,
        zcutsFolder, conf['InputType'] == 'mc', addResponse, nocutsFolder)

    # create pipelines for all bins
    if expandptbins:
        conf["Pipelines"] = ExpandPtBins(conf["Pipelines"], binning, True)

    #set the folder name
    for p, pval in conf["Pipelines"].items():
        ptVal = "NoBinning"

        if "ptbin" in pval["Filter"]:
            ptVal = "Pt" + str(pval["FilterPtBinLow"]) + "to" + str(pval["FilterPtBinHigh"])
            if onlyBasicQuantities:
                ReplaceWithQuantitiesBasic(pval)

        if "incut" in pval["Filter"]:
            if  pval["Cuts"] == ['muon_eta', 'muon_pt', 'zpt', 'zmass_window']:
                ptVal = ptVal + "_zcutsonly"
            elif 'leadingjet_eta' not in pval["Cuts"]:
                ptVal = ptVal + "_alleta"
            else:
                ptVal = ptVal + "_incut"

            if not ptVal == "NoBinning_incut":
                if onlyBasicQuantities:
                    ReplaceWithQuantitiesBasic(pval)
        else:
            ptVal = ptVal + "_allevents"

        pval["RootFileFolder"] = FolderPrefix + ptVal

    return conf


def StoreSettings(settings, filename):
    f = open(filename, "w")

    jsonOut = str(settings)
    # make it json conform
    jsonOut = jsonOut.replace("\'", "\"")

    try:
        import json
        # dont display config on console, it is annyoing
        #print json.dumps( settings, sort_keys=True, indent=4 )
        json.dump(settings, f, sort_keys=True, indent=4)
    except BaseException:
        f.write(jsonOut)
        print "No json Module found. Using fallback method ..."

    f.close()
    print len(settings["Pipelines"]), "pipelines configured."


def StoreGCDataset(settings, nickname, filename):
    print "Generating " + filename

    # ordering is important in the .dbs file format
    cfile = open(filename, 'wb')
    cfile.write("[" + nickname + "]\n")
    cfile.write("nickname = " + nickname + "\n")
    cfile.write("events = " + str(-len(settings['InputFiles'])) + "\n")

    path = os.path.split(settings['InputFiles'][0])[0]
    cfile.write("prefix = " + path + "\n")

    for f in settings['InputFiles']:
        cfile.write(os.path.split(f)[1] + " = -1\n")

    cfile.close()


def StoreGCConfig(settings, nickname, filename):
    print "Generating " + filename

    config = ConfigParser.RawConfigParser()
    # important, so the case is preserved
    config.optionxform = str
    config.add_section("global")
    config.set("global", "include", "gc_common.conf")
    config.set("global", "workdir space", "0")
    config.add_section("UserMod")
    config.set("UserMod", "dataset", nickname + " : " + nickname + ".dbs")

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
    config.set("global", "cmdargs", "-G -c")

    config.add_section("jobs")
    config.set("jobs", "in queue", 50)
    #config.set("jobs", "shuffle", True)
    config.set("jobs", "wall time", "1:50:00")
    config.set("jobs", "monitor", "scripts")
    #config.set("jobs", "memory", 3000)

    config.add_section("local")
    config.set("local", "queue", "short")
    config.set("local", "delay output", "True")

    config.add_section("UserMod")
    # use 40 jobs for the MC, 80 for data
    if settings["InputType"] == "mc":
        config.set("UserMod", "files per job", int(round(len(settings["InputFiles"]) / 80. + 0.4999)))
    else:
        config.set("UserMod", "files per job", int(round(len(settings["InputFiles"]) / 120. + 0.4999)))
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


def StoreMergeScript(settings, nickname, filename, output_folder, merge_folder='temp'):
    print "Generating", filename
    cfile = open(filename, 'wb')
    cfile.write("hadd " + output_folder + settings["OutputPath"] + ".root " + output_folder + nickname + "_job_*.root\n")
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)
    print "Generating", filename.replace("merge", "parallelmerge")
    mfile = open(filename.replace("merge", "parallelmerge"), 'wb')
    #mfile.write("mkdir -p " + output_folder.replace("out", merge_folder) + "\n")
    mfile.write(GetBasePath() + "scripts/parallelmerge.sh " + output_folder + " 10")
    os.chmod(filename.replace("merge", "parallelmerge"), stat.S_IRWXU)


def StoreShellRunner(settings, nickname, filename):
    print "Generating " + filename
    cfile = open(filename, 'wb')
    cfile.write("echo $FILE_NAMES\n")
    cfile.write("cd " + GetCMSSWPath() + "\n")
    if "CMSSW_5_" in GetCMSSWPath():
        cfile.write("export VO_CMS_SW_DIR=/wlcg/sw/cms\n")
        cfile.write("export SCRAM_ARCH=slc5_amd64_gcc462\n")
        cfile.write("source $VO_CMS_SW_DIR/cmsset_default.sh\n")
    else:
        cfile.write("source /wlcg/sw/cms/experimental/cmsset_default.sh\n")
    cfile.write("eval `scram runtime -sh`\n")
    cfile.write("cd -\n")
    cfile.write("cd " + GetBasePath() + "\n")
    cfile.write("source " + GetBasePath() + "scripts/ClosureEnv.sh\n")
    cfile.write("cd -\n")
    cfile.write(GetBasePath() + "closure " + GetBasePath() + "cfg/closure/" + nickname + ".py.json")
    cfile.close()
    os.chmod(filename, stat.S_IRWXU)


def Run(settings, arguments):
    """Run this config with closure

    The options are:
      --storeonly   Just generate the json config file and exit
      --fast n [m]  Run over a few files to see if it works (file number n to m)
      --batch       Split into jobs for processing on a cluster
      --skip n [m]  skip first n events and run over the next m events [m=1]
    """
    print "    ______     ___       __       __  .______    ___________    __    ____ "
    print "   /      |   /   \     |  |     |  | |   _  \  |   ____\   \  /  \  /   / "
    print "  |  ,----'  /  ^  \    |  |     |  | |  |_)  | |  |__   \   \/    \/   /  "
    print "  |  |      /  /_\  \   |  |     |  | |   _  <  |   __|   \            /   "
    print "  |  `----./  _____  \  |  `----.|  | |  |_)  | |  |       \    /\    /    "
    print "   \______/__/     \__\ |_______||__| |______/  |__|        \__/  \__/     "
    print ""

    # skip first n events and run over the next m events: --skip n [m=1]
    if len(arguments) > 2 and "--skip" in arguments:
        settings['SkipEvents'] = int(arguments[arguments.index("--skip") + 1])
        try:
            settings['EventCount'] = int(arguments[arguments.index("--skip") + 2])
        except:
            settings['EventCount'] = 1
        print "Running over %s events beginning with event %d." % (
            "all" if settings['EventCount'] < 0 else str(settings['EventCount']),
            settings['SkipEvents'])

    filename = arguments[0] + ".json"
    StoreSettings(settings, filename)

    base_path = GetBasePath()
    work_path = GetWorkPath()

    if len(arguments) > 1 and "--storeonly" in arguments:
        print "The settings were stored to", filename
        exit(0)

    if len(arguments) <= 1 or "--batch" not in arguments[1]:
        subprocess.call(["./closure", filename])
    else:
        nickname = os.path.split(filename)[1]
        nickname = nickname.split(".")[0]
        print "Generating GC configs with nickname " + nickname + " ..."
        # store the input files in gc format
        if not os.path.exists(work_path + "work/"):
            os.mkdir(work_path + "work/")
        if not os.path.exists(work_path + "work/" + nickname):
            os.mkdir(work_path + "work/" + nickname)
        if not os.path.exists(work_path + "work/" + nickname + "/out/"):
            os.mkdir(work_path + "work/" + nickname + "/out/")

        StoreGCDataset(settings, nickname, work_path + "work/" + nickname + "/" + nickname + ".dbs")
        StoreGCConfig(settings, nickname, work_path + "work/" + nickname + "/" + nickname + ".conf")
        StoreGCCommon(settings, nickname, work_path + "work/" + nickname + "/gc_common.conf", work_path + "work/" + nickname + "/out/")
        #StoreMergeScript(settings, nickname, work_path + "work/" + nickname + "/merge.sh", work_path + "work/" + nickname + "/out/")
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
