#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""ArtusConfigBase provides the tools to make a valid artus config.

The most used functions are:
  - BaseConfig to generate a default configuration
  - CreateFileList to create a list of input files
  - Run to acutally call artus and run it
"""
import copy
import glob
import socket
import ConfigParser
import os
import stat
import getpass
import json


def FindFileInList(conf, number):
    for f in conf['InputFiles']:
        if "_" + str(number) in f:
            print conf['InputFiles'].index(f)


def GetDefaultBinning():
    return [30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]


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


def BaseConfig(inputtype, run='2012', analysis='zmumu', tagged=True, rundepMC=False):
    """Basic configuration for Artus.

    Return a default configuration for Artus depending on

      - @param inputtype can be either 'data' or 'mc'. Default settings are adapted.
      - @param run can be either '2011' or '2012'. Parameters are set accordingly.
      - @param analysis can be either 'zmumu' or 'vbf'. The cuts are set accordingly.
    """
    config = {
        'SkipEvents': 0,
        'EventCount': -1,
        'GlobalProducer': [],
        'L1Correction': 'L1FastJet',
        'EnableMetPhiCorrection': False,
        'MetPhiCorrectionParameters': [],
        'EnablePuReweighting': False,
        'Enable2ndJetReweighting': False,
        'EnableSampleReweighting': False,
        'EnableLumiReweighting': False,
        'HcalCorrection': 0.0,
        'Jec': "data or mc?",
        'JsonFile': "year?",
        'InputType': inputtype,
        'InputFiles': [],     # overridden by artus
        'OutputPath': "out",  # overridden by artus
        'MuonID2011': (run == '2011'),
        'Pipelines': {
            'default': {
                'Level': 1,
                'JetAlgorithm': "AK5PFJetsCHSL1L2L3",
                'Consumer': ["ntuple"],
                'QuantitiesVector': [
                    "zpt", "zeta", "zy",
                    "zphi", "zmass", "npv", "rho",
                    "weight",
                    "jet1pt", "jet1eta", "jet1phi", "mpf", "rawmpf",
                    "METpt", "METphi", "rawMETpt", "rawMETphi", "sumEt", "jet1photonfraction",
                    "jet1chargedemfraction", "jet1chargedhadfraction", "jet1neutralhadfraction",
                    "jet1muonfraction", "jet1HFhadfraction", "jet1HFemfraction",
                    "jet2pt", "jet2eta", "jet2phi", "uept", "uephi", "ueeta",
                    "otherjetspt", "otherjetseta", "otherjetsphi", "njets", "njetsinv",
                    "unc", "nputruth",
                ]
            }
        },
        'MinBiasXS': 68.5,
        'Tagged': tagged,
        'VetoPileupJets': False,
        'checkKappa': False,
        'RC': False,  # also provide random cone offset JEC, and use for type-I

        # Wire kappa objects
        'electrons': 'electrons',
    }

    # electrons:
    if analysis == 'zee':
        # The order of these producers is important!
        config['GlobalProducer'] = [
            'valid_electron_producer', 'zee_producer', 'valid_jet_ee_producer',
            'jet_corrector', 'typeImet_producer', 'jet_sorter',
            'unclustered_energy_producer', 'leading_jet_uncertainty_producer',
        ]
        config['Pipelines']['default']['QuantitiesVector'] += [
			"nelectrons",
            "emass", "ept", "eeta",
            "eminusmass", "eminuspt", "eminuseta", "eminusphi", "eminusiso", "eminusid", "eminustrigid",
            "eplusmass", "epluspt", "epluseta", "eplusphi", "eplusiso", "eplusid", "eplustrigid",
            "eplusecaliso03", "eminusecaliso03", "eplusecaliso04", "eminusecaliso04",
        ]
        config['muons'] = ''
        config['electrons'] = 'electrons'
    else:
        # The order of these producers is important!
        config['GlobalProducer'] = [
            'valid_muon_producer', 'muon_corrector', 'z_producer',
            'valid_jet_producer', 'jet_corrector', 'typeImet_producer', 'jet_sorter',
            'unclustered_energy_producer', 'leading_jet_uncertainty_producer',
        ]
        config['Pipelines']['default']['QuantitiesVector'] += [
            "mupluspt", "mupluseta", "muplusphi",
            "muminuspt", "muminuseta", "muminusphi",
            "mu1pt", "mu1eta", "mu1phi",
            "mu2pt", "mu2eta", "mu2phi",
            "nmuons", "muplusiso", "muminusiso",
        ]
        config['muons'] = 'muons'
        config['electrons'] = ''

    if tagged:
        config['Pipelines']['default']['QuantitiesVector'] += [
            "qglikelihood", "qgmlp",
            "combinedsecondaryvertexbjettag", "combinedsecondaryvertexmvabjettag",
            "jet1puJetIDFullLoose", "jet1puJetIDFullMedium", "jet1puJetIDFullTight",
            "jet1puJetIDCutbasedLoose", "jet1puJetIDCutbasedMedium", "jet1puJetIDCutbasedTight",
            "jet2puJetIDFullLoose", "jet2puJetIDFullMedium", "jet2puJetIDFullTight",
            "jet2puJetIDCutbasedLoose", "jet2puJetIDCutbasedMedium", "jet2puJetIDCutbasedTight",
        ]
    config['Pipelines']['default'].update(GetCuts(analysis))

    if inputtype == 'data':
        config = SetDataSpecific(config, run, analysis)
    elif inputtype == 'mc':
        config = SetMcSpecific(config, run, analysis, rundepMC)
    else:
        print "The inputtype must be either 'data' or 'mc'."
        exit(1)

    return config


def SetMcSpecific(cfg, run='2012', analysis='zmumu', rundepMC=False):
    """Add Monte-Carlo specific settings to a config.

    The MC settings include

      - the set of Jet Energy Corrections
      - pile-up reweighting factors
      - additional producers
    """
    # general MC settings
    cfg['Pipelines']['default']['QuantitiesVector'] += [
                    "npu", "eff",
                    "genjet1pt", "genjet1eta", "genjet1phi", "genjet2pt",
                    "matchedgenjet1pt", "matchedgenjet2pt", "genmpf",
                    "algoflavour", "physflavour",
                    "algopt", "physpt",
                    "jet1ptneutrinos", "genjet1ptneutrinos", "mpfneutrinos", "neutralpt3", "neutralpt5",
                    "genmupluspt", "genmupluseta", "genmuplusphi",
                    "genmuminuspt", "genmuminuseta", "genmuminusphi",
                    "ngenmuons", "ngenphotons", "ngenphotonsclose", "closestphotondr", "nzs", "ninternalmuons", "nintermediatemuons", "ptgenphotonsclose", "ptdiff13", "ptdiff12", "ptdiff23",
                    "genzpt", "genzy", "genzmass", "deltaRzgenz"
    ]
    cfg['GlobalProducer'] += ['jet_matcher', 'gen_producer', 'gen_balance_producer', 'gen_met_producer', 'weight_producer', 'flavour_producer']
    cfg['AK5GenJets'] = 'AK5GenJetsNoNu'
    cfg['LHE'] = ''

    # Year-dependent settings
    if run == '2011':
        cfg['Jec'] = getPath() + "/data/jec/START44_V12"
        cfg["EnablePuReweighting"] = True
        cfg['PileupWeights'] = getPath() + "/data/pileup/weights_160404-180252_7TeV_ReRecoNov08_kappa539_MC11.root"
        cfg["MuonSmearing"] = True
        cfg["MuonRadiationCorrection"] = False
        cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2011_MC_44X.txt"
    elif run == '2012':
        cfg['Jec'] = getPath() + "/data/jec/Summer13_V5_MC"
        cfg['MetPhiCorrectionParameters'] = [0.1166, 0.0200, 0.2764, -0.1280]
        cfg["EnablePuReweighting"] = True
        if analysis == 'zee':
            cfg['PileupWeights'] = getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2014_01_31_zee_mc.root"
        else:
            cfg['PileupWeights'] = getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_68_5mb_kappa539_MC12_madgraph_tags.root"
        cfg["MuonSmearing"] = True
        cfg["MuonRadiationCorrection"] = False
        cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2012_MC_53X_smearReReco.txt"
    else:
        print "MC period", run, "is undefined. No jet corrections known."
        exit(0)

    # Analysis-specific settings
    if analysis is 'zee':
        cfg['Pipelines']['default']['QuantitiesVector'] += [
            "ngenelectrons", "ngeninternalelectrons", "ngenintermediateelectrons",
            "genepluspt", "genepluseta", "geneplusphi",
            "geneminuspt", "geneminuseta", "geneminusphi",
            "deltaReplusgeneplus", "deltaReminusgeneminus"
        ]
    else:
        cfg['EnableLumiReweighting'] = True
        cfg['EnableTriggerReweighting'] = True
        cfg['NEvents'] = 30459503
        cfg['XSection'] = 3503.71
    # special for rundep MC
    if rundepMC:
        cfg['Pipelines']['default']['QuantitiesVector'] += ['run', 'eventnr', 'lumisec']
    return cfg


def SetDataSpecific(cfg, run='2012', analysis='zmumu'):
    """Add data specific settings to a config

    The data settings include

      - the set of Jet Energy Corrections
      - json files
      - HLT paths
      - additional producers
    """
    d = {}
    if run == '2011':
        cfg['Jec'] = getPath() + "/data/jec/GR_R_44_V13"
        cfg['JsonFile'] = getPath() + "/data/json/Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON_v2.txt"
        cfg['HltPaths'] = [
            # Mu7 Trigger
            "HLT_DoubleMu7_v1", "HLT_DoubleMu7_v2", "HLT_DoubleMu7_v3", "HLT_DoubleMu7_v4", "HLT_DoubleMu7_v5",
            # Mu8 Trigger
            "HLT_Mu8_v16",
            # Mu13_Mu8 Trigger
            "HLT_Mu13_Mu8_v1", "HLT_Mu13_Mu8_v2", "HLT_Mu13_Mu8_v3", "HLT_Mu13_Mu8_v4", "HLT_Mu13_Mu8_v5",
            "HLT_Mu13_Mu8_v6", "HLT_Mu13_Mu8_v7", "HLT_Mu13_Mu8_v8", "HLT_Mu13_Mu8_v9", "HLT_Mu13_Mu8_v10",
            "HLT_Mu13_Mu8_v11", "HLT_Mu13_Mu8_v12", "HLT_Mu13_Mu8_v13", "HLT_Mu13_Mu8_v14",
            "HLT_Mu13_Mu8_v15", "HLT_Mu13_Mu8_v16", "HLT_Mu13_Mu8_v17", "HLT_Mu13_Mu8_v18",
            # Mu17_Mu8 Trigger
            "HLT_Mu17_Mu8_v10", "HLT_Mu17_Mu8_v11"
            ]
        cfg['PileupTruth'] = getPath() + "/data/pileup/2011_pumean_pixelcorr.txt"
        cfg["MuonSmearing"] = False
        cfg["MuonRadiationCorrection"] = False
        cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2011_DATA_44X.txt"
    elif run == '2012':
        cfg['Jec'] = getPath() + "/data/jec/Summer13_V5_DATA"
        cfg['MetPhiCorrectionParameters'] = [0.2661, 0.3217, -0.2251, -0.1747]
        cfg['JsonFile'] = getPath() + "/data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
        if analysis == 'zee':
            cfg['HltPaths'] = ["HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v%d" % v for v in range(15, 20)]
        else:
            cfg['HltPaths'] = ["HLT_Mu17_Mu8_v%d" % v for v in range(1, 30)]
        cfg['PileupTruth'] = getPath() + "/data/pileup/pumean_pixelcorr.txt"
        cfg["MuonRadiationCorrection"] = False
        cfg["MuonSmearing"] = False
        cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2012ABC_DATA_ReReco_53X.txt"
        cfg["MuonCorrectionParametersRunD"] = getPath() + "/data/muoncorrection/MuScleFit_2012D_DATA_ReReco_53X.txt"
    else:
        print "Run period", run, "is undefined. No json and jet corrections known."
        exit(1)

    cfg['Pipelines']['default']['QuantitiesVector'] += ['run', 'eventnr', 'lumisec']
    cfg['Pipelines']['default']['Filter'].append('json')
    cfg['Pipelines']['default']['Filter'].append('hlt')
    cfg['GlobalProducer'] += ['hlt_selector', 'pileuptruth_producer']
    return cfg


def GetCuts(analysis='zmumu'):
    """Return a set of default cuts for a given analysis."""
    cuts = {
        'zmumu': {
            'GenCuts': False,
            'Cuts': [
                'zpt',
                'leadingjet_pt',
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

            'Filter': ['valid_z', 'valid_jet', 'metfilter', 'incut'],
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

            'Filter': ['valid_z', 'valid_jet', 'metfilter', 'incut'],
        },
        'zee': {
            'GenCuts': False,
            'Cuts': [
                'electron_eta',
                'electron_pt',
                'zmass_window',
                'zpt',

                'leadingjet_pt',
                'back_to_back',
            ],
            'CutElectronEta': 2.4,
            'CutElectronPt': 25.0,

            'CutZMassWindow': 10.0,
            'CutZPt': 30.0,

            'CutLeadingJetEta': 1.3,
            'CutLeadingJetPt': 12.0,

            'CutSecondLeadingToZPt': 0.2,
            'CutBack2Back': 0.34,

            'Filter': ['valid_z', 'valid_jet', 'metfilter', 'incut'],
        }
    }
    if analysis not in cuts:
        print "There are no cuts defined for", analysis + "!"
        exit(1)
    return cuts[analysis]


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


def expand(config, variations=[], algorithms=[], default="default"):
    """create pipelines for each algorithm times each variation"""
    pipelines = config['Pipelines']
    p = config['Pipelines'][default]
    if p['JetAlgorithm'] not in algorithms:
        algorithms.append(p['JetAlgorithm'])
    if config['InputType'] == 'data' and "Res" not in p['JetAlgorithm']:
        algorithms.append(p['JetAlgorithm'] + "Res")

    #find global algorithms
    config["GlobalAlgorithms"] = []
    removelist = ["Jets", "L1", "L2", "L3", "Res", "Hcal", "Custom"]
    for algo in algorithms:
        for r in removelist:
            algo = algo.replace(r, "").replace("CHS", "chs")
        if algo not in config["GlobalAlgorithms"]:
            config["GlobalAlgorithms"].append(algo)

    # copy for variations
    for v in variations:
        if v == 'all':
            pipelines[v] = copy.deepcopy(p)
            pipelines[v]['Cuts'] = []
            if 'incut' in pipelines[v]['Filter']:
                    pipelines[v]['Filter'].remove('incut')
        if v == 'zcuts':
            pipelines[v] = copy.deepcopy(p)
            removelist = ['leadingjet_pt', 'back_to_back']
            for cut in removelist:
                if cut in pipelines[v]['Cuts']:
                    pipelines[v]['Cuts'].remove(cut)
        if v == 'fullcuts':
            pipelines[v] = copy.deepcopy(p)
            pipelines[v]['Cuts'].append('leadingjet_eta')
            pipelines[v]['Cuts'].append('secondleading_to_zpt')
            pipelines[v]['CutLeadingJetEta'] = 1.3
            pipelines[v]['CutSecondLeadingToZPt'] = 0.2

    # rename template pipline default to incut
    pipelines['incut'] = pipelines.pop(default)

    # copy for algorithms, naming scheme: incut_algo
    for name, p in pipelines.items():
        for algo in algorithms:
            pipelines[name + "_" + algo] = copy.deepcopy(p)
            pipelines[name + "_" + algo]["JetAlgorithm"] = algo
        del pipelines[name]

    for p in pipelines.values():
        p['QuantitiesString'] = ":".join(p['QuantitiesVector'])

    return config


def pipelinediff(config, to=None):
    print "Comparing", len(config['Pipelines']), "pipelines:"
    if to == None:
        to = filter(lambda x: 'incut' in x, config['Pipelines'].keys())[0]

    for name, p in config['Pipelines'].items():
        if name != to:
            print "- Compare", name, "to", to
            pipelinediff2(p, config['Pipelines'][to])
    print


def pipelinediff2(p1=None, p2=None):
    for k, v in p1.items():
        if k in p2.keys():
            if p1[k] != p2[k]:
                print "    different %s: %s != %s" % (k, str(p1[k]), str(p2[k]))

