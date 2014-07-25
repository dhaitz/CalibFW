import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', lhe=True)
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_02_28_zee_mc_powheg/*.root"
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 42718644
    conf['XSection'] = 1966.7
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

