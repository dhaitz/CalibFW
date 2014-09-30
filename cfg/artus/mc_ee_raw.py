import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', rundepMC=True, lhe=True)
    conf["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_07_18_ee-mm-mcRD/*.root",
        nafpath="/nfs/dust/cms/user/dhaitz/skims/2014_07_18_ee-mm-mcRD/*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 30458871
    conf['XSection'] = 3503.71

    conf['PileupWeights'] = base.getPath() + "/data/pileup/weights_190456-208686_8TeV_22Jan2013ReReco_2014_06_23_zee_madgraph.root"

    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf

