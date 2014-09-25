import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_08_08_data_WZ/kappa_WW_*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 10000431
    conf['XSection'] = 54.838
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
