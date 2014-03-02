import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_02_28_zee_backgrounds/*.root"
    #conf['EnableLumiReweighting'] = False
    conf['EnableTriggerReweighting'] = False
    #conf['checkKappa'] = "73f0d43"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

