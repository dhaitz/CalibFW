import ArtusConfigBase as base


def config():
    conf = base.BaseConfig('mc', '2012', rundepMC=True)
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa5313_MC12_madgraph_rundep/*.root"
    conf = base.expand(conf, ['all', 'zcuts'])
    return conf

