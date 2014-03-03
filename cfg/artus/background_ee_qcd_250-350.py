import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf['InputFiles'] = '/storage/a/dhaitz/skims/2014_02_28_zee_backgrounds/kappa_QCD_250-350_*.root'
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 34611322
    conf['XSection'] = 4250
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
