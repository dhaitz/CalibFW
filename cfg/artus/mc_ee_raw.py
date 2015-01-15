import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee', rundepMC=True, lhe=True)
    conf["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_07_18_ee-mm-mcRD/*.root",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2015_01_12_ee_mc_madgraphRD/kappa_madgraphRD_*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 30458871
    conf['XSection'] = 3503.71

    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2015_01_12_ee_madgraphRD.root"

    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf

