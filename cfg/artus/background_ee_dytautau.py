import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='zee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="/storage/9/dhaitz/skims/2014_07_31_ee_mc_DYtautau",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_07_31_ee_mc_DYtautau"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 3295238
    conf['XSection'] = 1966.7
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
