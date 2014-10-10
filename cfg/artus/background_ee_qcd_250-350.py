import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', analysis='ee')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_08_08_data_QCD/kappa_QCD_250-350_*.root"
    )
    conf['EnableLumiReweighting'] = True
    conf['EnableTriggerReweighting'] = False
    conf['NEvents'] = 34611322
    conf['XSection'] = 4250
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
