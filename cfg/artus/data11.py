import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2011')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data11/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
