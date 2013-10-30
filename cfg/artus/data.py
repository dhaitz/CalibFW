import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('data', '2012')
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_Data12_tags/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
