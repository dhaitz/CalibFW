import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee')
    conf['InputFiles'] = '/storage/a/dhaitz/skims/2014_02_28_zee_backgrounds/kappa_DYmumu_*.root'
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 48831381
    conf['XSection'] = 1966.7
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
