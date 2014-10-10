import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_08_08_data_QCD/kappa_QCD_170-250_*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 31697066
    conf['XSection'] = 30990
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
