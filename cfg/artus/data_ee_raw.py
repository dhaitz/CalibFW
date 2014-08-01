import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/8/dhaitz/skims/2014_06_23_ee-data/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
