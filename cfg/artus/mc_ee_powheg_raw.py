import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', lhe=True)
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_07_24_zee_mc_powheg/*.root"
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 3297045
    conf['XSection'] = 1966.7
    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2014_07_24_zee_powheg.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

