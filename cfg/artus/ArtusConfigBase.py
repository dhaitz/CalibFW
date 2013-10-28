#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""ArtusConfigBase provides the tools to make a valid artus config.

The most used functions are:
  - BaseConfig to generate a default configuration
  - CreateFileList to create a list of input files
  - Run to acutally call artus and run it
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


def GetBasePath(variable='EXCALIBUR_BASE'):
    """Return the path of the Excalibur repository (CalibFW)."""
    try:
        return os.environ[variable] + "/"
    except:
        print variable, "is not in shell variables:", os.environ.keys()
        print "Please source scripts/ini_excalibur!"
        exit(1)


def GetWorkPath():
    """Return work path if the shell variable 'EXCALIBUR_WORK' is set."""
    try:
        return os.environ['EXCALIBUR_WORK'] + "/"
    except:
        print "WorkPath is not set. BasePath is used instead."
        return GetBasePath()


def getPath(variable='EXCALIBUR_BASE', nofail=False):
    try:
        return os.environ[variable]
    except:
        print variable, "is not in shell variables:", os.environ.keys()
        print "Please source scripts/ini_excalibur and CMSSW!"
        if nofail:
            return None
        exit(1)


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
    """Basic configuration for Artus.

    Return a default configuration for Artus depending on

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
        'OutputPath': "out",  # "artus_" + inputtype + "_" + run,
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
        cfg['Jec'] = GetBasePath() + "data/jec/START44_V12"
        ApplyPUReweighting(cfg, 'mc11_160404-180252_7TeV_ReRecoNov08_v2')
    elif run == '2012':
        cfg['Jec'] = GetBasePath() + "data/jec/Summer13_V5_MC"
        cfg['MetPhiCorrectionParameters'] = [0.1166, 0.0200, 0.2764, -0.1280]
        ApplyPUReweighting(cfg, 'kappa539_MC12_madgraph_190456-208686_8TeV_22Jan2013ReReco')
    else:
        print "MC period", run, "is undefined. No jet corrections known."
        exit(0)

    cfg['GlobalProducer'] += ['jet_matcher', 'gen_producer', 'weight_producer', 'flavour_producer']
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
        cfg['Jec'] = GetBasePath() + "data/jec/GR_R_44_V13"
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
        cfg['Jec'] = GetBasePath() + "data/jec/Summer13_V5_DATA"
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
            elif conf['InputType'] == 'data':
                pval['QuantitiesVector'] += ['eventnr', 'lumisec']
        else:
            pval['QuantitiesVector'] = quantities

        pval['QuantitiesString'] = ":".join(pval['QuantitiesVector'])

        # replace the quantites_vector with integers according to the dictionary
        #new_quantities = []
        #for q in pval['QuantitiesVector']:
        #    new_quantities += [artus_dict[q]]
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
    calculated via scripts/weightCalc.py and stored in the following
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
    need it. The weight factors are calculated via scripts/weightCalc.py and
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
        print "Please add them in ArtusConfigBase or do not use Apply2ndJetReweighting."
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
            70.551230 / 200000,  # 15-20
            77.535330 / 150154,  # 20-30 (old sample)
            62.745670 / 150000,  # 30-50
            28.738060 / 100160,  # 50-80 (old sample)
            9.7459310 / 96000,   # 80-120
            2.8100250 / 98560,   # 120-170
            0.7702934 / 100000,  # 170-230
            0.2142680 / 96640,   # 230-300
            0.08858213 / 90517,  # 300-inf
        ],
    }

    if sample not in d:
        print "No sample weights for this dataset:", sample
        print "Weights are available for:", ", ".join(d.keys())
        print "Please add them in ArtusConfigBase or do not use ApplySampleReweighting."
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
