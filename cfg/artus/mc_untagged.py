import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', tagged=False)
    conf["InputFiles"] = "/storage/6/berger/zpj/kappa539_MC12_madgraph/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf
