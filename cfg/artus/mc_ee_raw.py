import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee', rundepMC=True, lhe=True)
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_07_18_mc_ee_mm/*.root"
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 30459503
    conf['XSection'] = 3503.71


    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

