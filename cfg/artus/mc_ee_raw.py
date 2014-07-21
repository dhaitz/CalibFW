import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', rundepMC=True)
    #conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_06_23_zee_mc_madgraph/*.root"
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_07_18_mc_ee_mm/*.root"
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 30459503
    conf['XSection'] = 3503.71

    conf['GlobalProducer'] += ['lhe_producer']
    conf['LHE'] = 'LHE'
    conf['Pipelines']['default']['QuantitiesVector'] += ['lhezpt', 'lhezeta', 'lhezy', 'lhezphi', 'lhezmass']

    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

