import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012', analysis='zee')
    conf["InputFiles"] = "/storage/a/dhaitz/skims/2014_06_23_zee_data/*.root"
    conf = base.expand(conf, ['all', 'zcuts', 'incut'])
    return conf
