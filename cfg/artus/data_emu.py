import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zemu')
    conf["InputFiles"] = base.setInputFiles(
        ekppath="",
        nafpath="/pnfs/desy.de/cms/tier2/store/user/dhaitz/2014_08_08_data_MuEG"
    )
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
