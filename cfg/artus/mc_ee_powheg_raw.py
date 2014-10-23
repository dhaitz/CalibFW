import ArtusConfigBase as base

def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee', lhe=True)
    conf["InputFiles"] = base.setInputFiles(
        ekppath = "",
        nafpath = "/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_10_20_ee_mc_powheg"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 47244567
    conf['XSection'] = 1966.7
    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2014_10_20_ee_powheg.root"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf

