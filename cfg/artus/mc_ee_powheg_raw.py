import ArtusConfigBase as base

def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee', lhe=True)
    conf["InputFiles"] = base.setInputFiles(
        ekppath = "/storage/a/dhaitz/skims/2014_07_24_zee_mc_powheg/*.root",
        nafpath = "/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_07_30_ee_mc_powheg"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 3297045
    conf['XSection'] = 1966.7
    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2014_07_24_zee_powheg.root"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf

