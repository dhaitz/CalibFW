import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf['InputFiles'] = '/storage/a/dhaitz/skims/2014_02_28_zee_backgrounds/kappa_tt_*.root'
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 21675970
    conf['XSection'] = 245.8
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
