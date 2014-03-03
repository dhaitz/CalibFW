import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_01_31_zee_mc/*.root"
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 30459503
    conf['XSection'] = 3503.71
    conf['checkKappa'] = "73f0d43"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

