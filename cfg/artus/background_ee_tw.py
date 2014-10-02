import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_09_30_ee_mc_tW"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 991118
    conf['XSection'] = 22.2
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
